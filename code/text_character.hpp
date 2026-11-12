#pragma once

#include "graphics/opengl/texture.hpp"

struct text_character
{
    graphics::opengl::Texture texture;

    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t   advance;
};