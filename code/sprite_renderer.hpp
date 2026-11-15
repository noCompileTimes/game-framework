#pragma once

#include "graphics/opengl/buffer.hpp"

// TODO when we will have a resources/graphics manager drop the transform_ubo as parameter
// TODO add a rotation based on some angle? it would be cool
inline auto render_sprite(const float x, const float y, const uint32_t index, const bool flipped, const graphics::opengl::Buffer& transform_ubo) -> void
{
    glm::mat4 current_sprite_matrix { 1.0f }; // TODO maybe join it with translation?

    current_sprite_matrix = translate(current_sprite_matrix, glm::vec3(x, y, 0.0f));

    if (flipped)
    {
        current_sprite_matrix = scale(current_sprite_matrix, glm::vec3(-1.0f, 1.0f, 1.0f));
    }

    transform_ubo.subdata(0, sizeof(glm::mat4), &current_sprite_matrix);

    graphics::opengl::Commands::draw_elements(GL_TRIANGLES, index * 6, 6);
}