#include "font_packer.hpp"
#include "resources.hpp"
#include "sprite_geometry.hpp"

#include "graphics/opengl/framebuffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/pipeline.hpp"
#include "graphics/opengl/commands.hpp"

#include "data/camera.hpp"

#include "core/rgba.hpp"

FontPacker::FontPacker(const std::string& folder, const uint32_t width, const uint32_t height)
    : _folder { folder }
    , _width { width}
    , _height { height }
{
}

// TODO use here a font? which already has the images as member?
auto FontPacker::init(const std::vector<image_data>& images) -> void
{
    _images = images;

    pack();
}

auto FontPacker::pack() -> void
{
    // TODO this seems like a common things between font_packer and sprite_packer
    //      start from here

    stbrp_context stbrp_context;

    std::vector<stbrp_node> atlas_nodes(_width);

    stbrp_init_target(&stbrp_context, static_cast<int32_t>(_width), static_cast<int32_t>(_height), atlas_nodes.data(), static_cast<int32_t>(atlas_nodes.size()));

    const auto rectangles_count = static_cast<int32_t>(_images.size());
    _rectangles.resize(rectangles_count);

    constexpr auto atlas_padding = 2;

    for (auto i = 0; i < _images.size(); i++)
    {
        const auto& image_data = _images[i];

        _rectangles[i].w = image_data.width  + atlas_padding;
        _rectangles[i].h = image_data.height + atlas_padding;
    }

    // stbrp_pack_rects places the rectangles sorted by height then width (and gives them back in our order after),
    // glyphs with the exact same size (ex: n and u in many fonts) are a tie for that sort and qsort is not stable,
    // so their placement in the atlas is arbitrary (the result is still correct, the txt keeps the character order)
    // TODO if we want a predictable visual order in the atlas, define STBRP_SORT (in stb.c) with a stable sort

    const auto all_packed = stbrp_pack_rects(&stbrp_context, _rectangles.data(), rectangles_count);
    std::println("{} {}", _folder, all_packed ? "packed successfully" : "few rectangles did not fit.");

    for (auto& rectangle : _rectangles)
    {
        rectangle.x += atlas_padding / 2; // TODO we should do a variable out of atlas_pading / 2? outside the loop? - same for sprite_packer
        rectangle.y += atlas_padding / 2;
        rectangle.w -= atlas_padding;
        rectangle.h -= atlas_padding;
    }

    // TODO this seems like a common things between font_packer and sprite_packer
    //      ends here

    std::ofstream output_file;
    output_file.open(std::string(_folder) + ".txt");

    // TODO maybe strip the "textures/" part from it
    output_file << _folder + "_atlas.png" << std::endl;

    output_file << rectangles_count << std::endl;

    // the txt rects (x y w h) are in pixels with the origin at the top-left of the atlas png
    // when turning a rect into UVs, the v axis depends on which texture you sample:
    //  - the png loaded with stbi_load (no vertical flip): row 0 is the top, so
    //      u0 = x / W, u1 = (x + w) / W, v_top = y / H, v_bottom = (y + h) / H
    //  - FontPacker::texture (rendered by build()): the top of the atlas sits at v = 1 (opengl bottom-up), so
    //      u0 = x / W, u1 = (x + w) / W, v_top = 1 - y / H, v_bottom = 1 - (y + h) / H
    //    this is why the atlas viewer in main uses create_sprite(..., true)
    // mixing these up draws the glyphs upside down (and takes the wrong rows from the atlas)

    for (auto i = 0; i < rectangles_count; ++i)
    {
        auto& [w, h, x, y, was_packed] = _rectangles[i];

        // a rect that did not fit keeps its line and quad as an empty rect, so the indexes of the next glyphs do not shift
        if (!was_packed)
        {
            std::println("FAILED to pack [Size: {}x{}]", w, h);

            x = 0;
            y = 0;
            w = 0;
            h = 0;
        }

        // TODO move this font atlas write into a separate function/class
        // TODO find a better/best way to write this in a file
        output_file << x << " " << y << " " << w << " " << h << std::endl;

        _geometry.append(create_sprite({ 0, 0,  static_cast<float>(w), static_cast<float>(h) }, false));
    }
}

auto FontPacker::build(const graphics::opengl::ShaderProgram& shader, const graphics::opengl::TextureSampler& sampler,
                         const graphics::opengl::Buffer& camera_ubo, const graphics::opengl::Buffer& transform_ubo) -> void
{
    constexpr auto channels = 1;
        const auto buffer_size = _width * _height * channels;

    // framebuffer - begin

    texture.create();
    texture.allocate(_width, _height, GL_R8, 1);

    //graphics::opengl::Renderbuffer renderbuffer;
    //renderbuffer.create();
    //renderbuffer.allocate(atlas_width, atlas_height, GL_DEPTH_COMPONENT24);

    graphics::opengl::Framebuffer framebuffer;
    framebuffer.create();
    framebuffer.attach(texture, GL_COLOR_ATTACHMENT0, 0);
    //framebuffer.attach(renderbuffer, GL_DEPTH_ATTACHMENT);

    // framebuffer - end

    graphics::opengl::Buffer framebuffer_pbo;
    framebuffer_pbo.create();
    framebuffer_pbo.allocate(buffer_size, nullptr, GL_MAP_READ_BIT | GL_CLIENT_STORAGE_BIT);
    framebuffer_pbo.bind(GL_PIXEL_PACK_BUFFER);

    // glyph quads

    graphics::opengl::Buffer sprites_vbo;
    sprites_vbo.create();
    sprites_vbo.allocate(sizeof(sprite_vertex) * _geometry.vertices.size(), _geometry.vertices.data(), 0);

    graphics::opengl::Buffer sprites_ebo;
    sprites_ebo.create();
    sprites_ebo.allocate(sizeof(uint32_t) * _geometry.elements.size(), _geometry.elements.data(), 0);

    graphics::opengl::VertexArray sprites_vao;
    sprites_vao.create();

    sprites_vao.attach_vertices(sprites_vbo, sizeof(sprite_vertex));
    sprites_vao.attach_elements(sprites_ebo);

    sprites_vao.attach_attribute({ 0, offsetof(sprite_vertex, position), 2, GL_FLOAT, 0 });
    sprites_vao.attach_attribute({ 1, offsetof(sprite_vertex, texcoord), 2, GL_FLOAT, 0 });

    constexpr core::rgba framebuffer_color { 0.0f, 0.0f, 0.0f, 0.0f };

    framebuffer.clear(&framebuffer_color.r, 0);
    //framebuffer.clear(1.0f);
    framebuffer.bind();

    graphics::opengl::Commands::viewport(0, 0, _width, _height);

    graphics::opengl::Pipeline::disable(GL_BLEND);

    // rectangles

    data::camera sprite_camera_data;
    sprite_camera_data.view       = glm::mat4(1.0f);
    sprite_camera_data.projection = glm::ortho(0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 0.0f, 1.0f);

    camera_ubo.upload(0, sizeof(data::camera), &sprite_camera_data);

    shader.bind();
    // TODO use a dedicated GL_NEAREST sampler for the packing pass - the shared sprite sampler is GL_LINEAR, which is only
    //      an exact copy while every quad lands on whole pixels at 1:1 scale (any half pixel offset or scaling blurs the atlas)
    sampler.bind(0);

    sprites_vao.bind();

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 1);

    for (auto i = 0; i < _images.size(); i++)
    {
        auto& [w, h, x, y, was_packed] = _rectangles[i];

        if (!was_packed || w == 0 || h == 0)
        {
            continue;
        }

        // TODO remove it if not needed later on
        auto parent_matrix = translate(glm::mat4(1.0f), { 0.0, 0.0, 0 });

        auto rectangle_matrix = translate(parent_matrix, { x, y, 0.0f });
        transform_ubo.upload(0, sizeof(glm::mat4), &rectangle_matrix);

        const auto& image = _images[i];

        graphics::opengl::Texture rectangle_texture { GL_TEXTURE_2D }; // TODO cache this into a set/map to not recreate it everytime if is the same size
        rectangle_texture.create();
        rectangle_texture.allocate(w, h, GL_R8, 1);
        rectangle_texture.upload(w, h, GL_RED, 0, image.pixels);
        rectangle_texture.bind(0); // TODO use a constant here? like albedo?

        // TODO make a constant somewhere for this 6 to know about the elements of a sprite
        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, i * 6, 6);

        rectangle_texture.destroy();
    }

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 4);

    graphics::opengl::Commands::read_pixels(0, 0, _width, _height, GL_RED, GL_UNSIGNED_BYTE, buffer_size);

    if (const auto* pixels = static_cast<uint8_t*>(framebuffer_pbo.map(0, buffer_size, GL_MAP_READ_BIT)))
    {
        stbi_flip_vertically_on_write(1);
        stbi_write_png((_folder + "_atlas.png").c_str(), _width, _height, channels, pixels, _width * channels);

        framebuffer_pbo.unmap();
    }

    // the glyph pixels are only borrowed from the font (Font::release_images frees them), just drop our copies of the pointers
    _images.clear();

    // TODO when we will have a resource/graphics manager maybe reuse these instead of delete? maybe it will save some time or something else

    sprites_vao.destroy();
    sprites_ebo.destroy();
    sprites_vbo.destroy();

    framebuffer_pbo.destroy();
    framebuffer.destroy();
}
