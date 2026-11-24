#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"
#include "graphics/opengl/buffer.hpp"

#include "geometry.hpp"
#include "vertex_types.hpp"
#include "sprite.hpp"

class FontAtlas
{
public:
    auto    init(std::string_view filename) -> void;
    auto release() const -> void;

    auto bind() const -> void;

    // appends the glyph quads of one text into the shared vbo/ebo, returns the start (in elements) to draw it from, nullopt if the buffers are full
    // TODO use text_vertex once create_sprite can build it (same layout as sprite_vertex for now)
    auto add(const geometry<sprite_vertex, uint32_t>& text_geometry) -> std::optional<uint32_t>;

    std::vector<sprite> sprites;

    uint32_t width  { };
    uint32_t height { };

private:
    static constexpr uint32_t max_glyphs = 2048; // for all the texts together, 4 vertices + 6 elements per glyph

    graphics::opengl::Texture _texture { GL_TEXTURE_2D };
    graphics::opengl::VertexArray _vao;

    graphics::opengl::Buffer _vbo;
    graphics::opengl::Buffer _ebo;

    // where the next text geometry is written in the shared buffers
    uint32_t _vertex_count  { };
    uint32_t _element_count { };
};

// TODO SDF/MSDF glyph | have a look at some point into this topic