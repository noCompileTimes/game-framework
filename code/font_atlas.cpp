#include "font_atlas.hpp"
#include "geometry.hpp"
#include "geometry_utils.hpp"
#include "vertex_types.hpp"
#include "sprite.hpp"
#include "resources.hpp"
#include "graphics/opengl/pipeline.hpp"

// TODO refactor/improve it at some point

// TODO maybe a better name? read/load/etc
auto FontAtlas::init(std::string_view filename) -> void
{
    std::ifstream sprite_atlas_file { filename.data() };

    if (!sprite_atlas_file)
    {
        std::println("Failed to open font atlas '{}'", filename);

        return;
    }

    std::string texture_filename;
    sprite_atlas_file >> texture_filename;

    const auto sprite_atlas_image = load_image(texture_filename);

    uint32_t rectangles_count { }; // initialized: if the read fails we draw nothing instead of looping over garbage
    sprite_atlas_file >> rectangles_count;

    //geometry<sprite_vertex, uint32_t> sprites_geometry;

    for (auto i = 0; i < rectangles_count; i++)
    {
        sprite sprite;
        sprite_atlas_file >> sprite.x >> sprite.y >> sprite.w >> sprite.h;

        sprites.emplace_back(sprite);

        // TODO this can be made in another for loop after we are done with the reading? make sense?
        //sprites_geometry.append(create_sprite(sprite_pivot::top_left, sprite.x, sprite.y, sprite.w, sprite.h,
        //    sprite_atlas_image.width, sprite_atlas_image.height, false));
    }

    constexpr auto magic_number = 10000;

    graphics::opengl::Buffer sprites_vbo; // TODO keep a private reference to them, maybe destroy it at some point in a release function
    sprites_vbo.create();
    sprites_vbo.allocate(sizeof(sprite_vertex) * magic_number, nullptr, 0);

    graphics::opengl::Buffer sprites_ebo; // TODO keep a private reference to them, maybe destroy it at some point in a release function
    sprites_ebo.create();
    sprites_ebo.allocate(sizeof(uint32_t) * magic_number, nullptr, 0);

    _vao.create();

    _vao.attach_vertices(sprites_vbo, sizeof(text_vertex));
    _vao.attach_elements(sprites_ebo);

    _vao.attach_attribute({ 0, offsetof(text_vertex, position), 2, GL_FLOAT, 0 });
    _vao.attach_attribute({ 1, offsetof(text_vertex, texcoord), 2, GL_FLOAT, 0 });

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 1);

    _texture.type = GL_TEXTURE_2D;
    _texture.create();
    _texture.allocate(sprite_atlas_image.width, sprite_atlas_image.height, GL_R8, 1);
    _texture.upload(sprite_atlas_image.width, sprite_atlas_image.height, GL_RED, 0, sprite_atlas_image.pixels);

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 4);
}

auto FontAtlas::bind() const -> void
{
    _texture.bind(0);
        _vao.bind();
}

// TODO create at some point some SpriteAtlasBatcher in which you can have just one vbo, ebo, vao which includes multiple
//      sprite atlases and we offset the indexes for the sprites - better from the performance perspective