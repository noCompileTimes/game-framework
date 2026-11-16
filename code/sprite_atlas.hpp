#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"

class SpriteAtlas
{
public:
    auto init(std::string_view filename) -> void;
    auto bind() const -> void;

private:
    graphics::opengl::Texture _texture;
    graphics::opengl::VertexArray _vao;
};