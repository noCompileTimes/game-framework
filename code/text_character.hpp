#pragma once

struct text_character // TODO maybe this should be called font_character
{
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t   advance;
    uint32_t   index;
};