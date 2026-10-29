#pragma once

#include <stb_image.h>

#include <string_view>

struct texture_data
{
    int32_t width;
    int32_t height;
    int32_t channels;

    uint8_t* pixels;
};

static auto load_texture(const std::string_view filename) -> texture_data
{
    texture_data data;

    data.pixels = stbi_load(filename.data(), &data.width, &data.height, &data.channels, 0);

    return data;
}