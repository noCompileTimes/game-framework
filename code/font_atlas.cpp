#include "font_atlas.hpp"
#include "geometry_utils.hpp"

#include "resources.hpp"
#include "graphics/opengl/pipeline.hpp"

// TODO refactor/improve it at some point

// TODO maybe a better name? read/load/etc
auto FontAtlas::init(std::string_view filename) -> void
{
    std::ifstream font_atlas_file { filename.data() };

    if (!font_atlas_file)
    {
        std::println("Failed to open font atlas '{}'", filename);

        return;
    }

    std::string texture_filename;
    font_atlas_file >> texture_filename;

    const auto font_atlas_image = load_image(texture_filename);

    width  = font_atlas_image.width;
    height = font_atlas_image.height;

    uint32_t rectangles_count { }; // initialized: if the read fails we draw nothing instead of looping over garbage
    font_atlas_file >> rectangles_count;

    for (auto i = 0; i < rectangles_count; i++)
    {
        sprite sprite;
        font_atlas_file >> sprite.x >> sprite.y >> sprite.w >> sprite.h;

        sprites.emplace_back(sprite);
    }

    _vbo.create();
    _vbo.allocate(sizeof(text_vertex) * max_glyphs * 4, nullptr, GL_DYNAMIC_STORAGE_BIT);

    _ebo.create();
    _ebo.allocate(sizeof(uint32_t) * max_glyphs * 6, nullptr, GL_DYNAMIC_STORAGE_BIT);

    _vao.create();

    _vao.attach_vertices(_vbo, sizeof(text_vertex));
    _vao.attach_elements(_ebo);

    _vao.attach_attribute({ 0, offsetof(text_vertex, position), 2, GL_FLOAT, 0 });
    _vao.attach_attribute({ 1, offsetof(text_vertex, texcoord), 2, GL_FLOAT, 0 });

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 1);

    _texture.create();
    _texture.allocate(width, height, GL_R8, 1);
    _texture.upload(width, height, GL_RED, 0, font_atlas_image.pixels);

    stbi_image_free(font_atlas_image.pixels);

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 4);
}

auto FontAtlas::release() const -> void
{
        _vao.destroy();

        _ebo.destroy();
        _vbo.destroy();

    _texture.destroy();
}

auto FontAtlas::bind() const -> void
{
    _texture.bind(0);
        _vao.bind();
}

auto FontAtlas::add(const geometry<sprite_vertex, uint32_t>& text_geometry) -> std::optional<uint32_t>
{
    // TODO this could not be done with geometry append?
    const auto vertices_count = static_cast<uint32_t>(text_geometry.vertices.size());
    const auto elements_count = static_cast<uint32_t>(text_geometry.elements.size());

    if (_vertex_count + vertices_count > max_glyphs * 4 || _element_count + elements_count > max_glyphs * 6)
    {
        std::println("Font atlas buffers are full, the text was not added (max {} glyphs)", max_glyphs);

        return std::nullopt;
    }

    // the text geometry elements start from 0, shift them to where its vertices land in the shared vbo
    std::vector<uint32_t> elements;
    elements.reserve(elements_count);

    for (const auto element : text_geometry.elements)
    {
        elements.emplace_back(element + _vertex_count);
    }

    _vbo.upload(_vertex_count  * sizeof(sprite_vertex), vertices_count * sizeof(sprite_vertex), text_geometry.vertices.data());
    _ebo.upload(_element_count * sizeof(uint32_t),      elements_count * sizeof(uint32_t),      elements.data());

    const auto start = _element_count;

    _vertex_count  += vertices_count;
    _element_count += elements_count;

    return start;
}

// TODO create at some point some SpriteAtlasBatcher in which you can have just one vbo, ebo, vao which includes multiple
//      sprite atlases and we offset the indexes for the sprites - better from the performance perspective