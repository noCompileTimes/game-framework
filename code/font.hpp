#pragma once

#include "text_character.hpp" // TODO maybe this should be called font_character?

#include "image_data.hpp"

class Font
{
public:
    auto load(const std::string& filename) -> int32_t;

    auto release_images() -> void;

    // TODO hide all the public members
    std::map<GLchar, text_character> characters;

    // TODO maybe make this pixels part of the text_character? maybe rename it to font_character? makes sense
    std::vector<image_data> images;

    int32_t baseline { };
};
