#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"

class SpriteAtlas
{
public:
    auto init(std::string_view filename) -> void;
    auto bind() const -> void; // TODO split this into bind_texture and bind_geometry (aka vao binding)?

private:
    graphics::opengl::Texture _texture;
    graphics::opengl::VertexArray _vao;
};