#pragma once

#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/texture.hpp"
#include "graphics/opengl/buffer.hpp"

class SpriteAtlas
{
public:
    auto    init(std::string_view filename) -> void;
    auto release() const -> void;

    auto bind() const -> void; // TODO split this into bind_texture and bind_geometry (aka vao binding)?

private:
    graphics::opengl::Texture _texture { GL_TEXTURE_2D };
    graphics::opengl::VertexArray _vao;

    graphics::opengl::Buffer _vbo;
    graphics::opengl::Buffer _ebo;
};