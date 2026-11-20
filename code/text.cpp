#include "text.hpp"

#include "geometry.hpp"
#include "geometry_utils.hpp"
#include "vertex_types.hpp"

auto Text::build(const Font& font, FontAtlas& font_atlas) -> void
{
    auto x = 0.0f;

    geometry<sprite_vertex, uint32_t> sprites_geometry;

    for (std::string::const_iterator c = text.begin(); c != text.end(); ++c)
    {
        const auto character = font.characters.find(*c);

        if (character == font.characters.end()) // not in the font (Font::load skips the characters the font has no glyph for)
        {
            continue;
        }

        auto [size, bearing, advance, index] = character->second;

        // the baseline sits below the top of the tallest glyph, so the text anchor (0, 0) is the top-left corner instead of the baseline
        const auto y = static_cast<float>(font.baseline);

        const auto xpos = x + bearing.x;
        const auto ypos = y - bearing.y;

        const auto w = size.x;
        const auto h = size.y;

        const auto& sprite = font_atlas.sprites[index];

        // the quad is placed already shifted (bearing + advance), the uvs pick the glyph from the atlas
        const core::rect position { { xpos, ypos }, { xpos + w, ypos + h } };

        // TODO create_text_sprite? to handle the font sprites character with the right vertex type
        sprites_geometry.append(create_sprite(position, sprite.x, sprite.y, sprite.w, sprite.h,
            static_cast<float>(font_atlas.width), static_cast<float>(font_atlas.height), false)); // false: the font png is loaded top-down

        x += static_cast<float>(advance >> 6); // 26.6 fixed point to pixels
    }

    if (const auto text_start = font_atlas.add(sprites_geometry))
    {
        start = *text_start;
        count = static_cast<uint32_t>(sprites_geometry.elements.size());
    }
}
