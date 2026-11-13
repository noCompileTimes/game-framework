#pragma once

#include "geometry.hpp"
#include "vertex_types.hpp"
#include "texture_data.hpp"

// TODO maybe call it SpriteAtlasPacker? - check the english
// TODO maybe at some point this can be extended from a tool class
class SpritePacker
{
public:
    auto  init(std::string_view folder, uint32_t width, uint32_t height) -> void;
    auto build() -> void;

    std::vector<texture_data> images_data;
    std::vector<stbrp_rect>   rectangles;

    geometry<sprite_vertex, uint32_t> rectangles_geometry;

private:
    uint32_t _width;
    uint32_t _height;
};