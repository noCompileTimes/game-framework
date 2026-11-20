#pragma once

#include "font.hpp"
#include "font_atlas.hpp"

class Text // TODO this should be called StaticText?
{
public:
    std::string text;

    // where the glyph quads of this text are in the font atlas ebo (in elements), drawn with one draw call by render_text
    uint32_t start { };
    uint32_t count { };

    // TODO rebuilding a text (changed string) would need its own slot in the font atlas buffers, for now build once
    auto build(const Font& font, FontAtlas& font_atlas) -> void;
};

// TODO maybe add a separate DynamicText which kind of can update some portion of the text in the buffer direclty?
//      maybe it can also allocate some extra characters in case the text is longer