#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"

#include "sprite.hpp"

class FontAtlas
{
public:
    auto init(std::string_view filename) -> void;
    auto bind() const -> void;

    std::vector<sprite> sprites;

private:
    graphics::opengl::Texture _texture;
    graphics::opengl::VertexArray _vao;
};

// TODO SDF/MSDF glyph | have a look at some point into this topic