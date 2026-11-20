#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"

class FontAtlas
{
public:
    auto init(std::string_view filename) -> void;
    auto bind() const -> void;

private:
    graphics::opengl::Texture _texture;
    graphics::opengl::VertexArray _vao;
};

// TODO SDF/MSDF glyph | have a look at some point into this topic