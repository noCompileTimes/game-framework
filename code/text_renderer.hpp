#pragma once

#include "text.hpp"

#include "graphics/opengl/buffer.hpp"
#include "graphics/opengl/commands.hpp"

// the glyph quads were baked by Text::build into the font atlas buffers, so a whole text is one draw call
// x, y  -> the top-left corner of the text on the screen, in pixels
// scale -> uniform, 1.0 is the size the font was loaded with (48px)
// the font atlas must be bound before (FontAtlas::bind)
// TODO when we will have a resources/graphics manager drop the transform_ubo as parameter
// TODO add rotation (like render_sprite) when we need it
inline auto render_text(const Text& text, const float x, const float y, const float scale, const graphics::opengl::Buffer& transform_ubo) -> void
{
    if (text.count == 0)
    {
        return;
    }

    glm::mat4 text_matrix { 1.0f };

    text_matrix = translate(text_matrix, glm::vec3 { x, y, 0.0f });

    if (scale != 1.0f)
    {
        text_matrix = glm::scale(text_matrix, glm::vec3 { scale, scale, 1.0f });
    }

    transform_ubo.upload(0, sizeof(glm::mat4), &text_matrix);

    graphics::opengl::Commands::draw_elements(GL_TRIANGLES, static_cast<int32_t>(text.start), static_cast<int32_t>(text.count));
}
