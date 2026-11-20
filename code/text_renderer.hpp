#pragma once

#include "font.hpp"
#include "font_atlas.hpp"

static auto render_text(const FontAtlas& font_atlas, const Font& font, std::string text) -> void
{
    auto x = 0.0f;

    for (std::string::const_iterator c = text.begin(); c != text.end(); ++c)
    {
        const auto character = font.characters.find(*c);

        if (character == font.characters.end()) // not in the font (Font::load skips the characters the font has no glyph for)
        {
            continue;
        }

        /*auto [texture, size, bearing, advance] = character->second;

        // the baseline sits at the font ascender, so the text anchor (0, 0) is the top-left corner instead of the baseline
        const auto y = static_cast<float>(font.ascender);

        const auto xpos = x + bearing.x;
        const auto ypos = y - bearing.y;

        const auto w = size.x;
        const auto h = size.y;

        const float vertices[6][4]
        {
            { xpos,     ypos,       0.0f, 0.0f },
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },

            { xpos,     ypos,       0.0f, 0.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };*/

        //vbo.subdata(0, sizeof(vertices), vertices);

        auto index = character->second.index;

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, index * 6, 6); // the atlas vao is indexed (4 vertices + 6 elements per glyph)

        //x += advance >> 6;

        index++;
    }
}

// TODO - fix the text with top left "anchor", it seems that it has some padding/margin on top of the text
// TODO - optimization | put all the characters inside the vertex buffer object