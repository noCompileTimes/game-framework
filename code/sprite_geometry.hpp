#pragma once

#include "core/rect.hpp"

#include "sprite_pivot.hpp"
#include "vertex_types.hpp"

#include "geometry.hpp"

// TODO check if this flip_vertical parameter it makes what stbi_set_flip_vertically_on_load does for stb image on load
//      flip_vertical = true  -> for textures stored bottom-up (e.g. framebuffer / render target textures)
//      flip_vertical = false -> for textures loaded with stb_image (stored top-down)
static auto create_sprite(const core::rect& position, const bool flip_vertical) -> geometry<sprite_vertex, uint32_t>
{
    const auto v_top    = flip_vertical ? 1.0f : 0.0f;
    const auto v_bottom = flip_vertical ? 0.0f : 1.0f;

    geometry<sprite_vertex, uint32_t> geometry
    {
        .vertices
        {
            { position.min, { 0.0f, v_top } },
            { { position.max.x, position.min.y }, { 1.0f, v_top } },
            {  position.max, { 1.0f, v_bottom } },
            { { position.min.x, position.max.y }, { 0.0f, v_bottom } }
        },
        .elements
        {
            0, 2, 1,
            0, 3, 2
        }
    };

    return geometry;
}

// same as above, but the uvs only cover one sprite of an atlas instead of the full texture
// x, y, width, height    -> the sprite rect inside the atlas, in pixels, origin at the top-left (as written by the SpritePacker txt)
// atlas_width/height     -> the size of the whole atlas texture, in pixels
// flip_vertical = true  -> for atlases stored bottom-up (e.g. SpritePacker::texture, rendered into a framebuffer)
// flip_vertical = false -> for atlases loaded with stb_image (stored top-down)
// TODO here we can drop the width  and height for the sprite? cause it can be computed from the rect min/max position?
static auto create_sprite(const core::rect& position, const float x, const float y, const float width, const float height,
                          const float atlas_width, const float atlas_height, const bool flip_vertical) -> geometry<sprite_vertex, uint32_t>
{
    const auto u_left   = x / atlas_width;
    const auto u_right  = (x + width) / atlas_width;

    const auto v_top    = flip_vertical ? 1.0f - y / atlas_height            : y / atlas_height;
    const auto v_bottom = flip_vertical ? 1.0f - (y + height) / atlas_height : (y + height) / atlas_height;

    geometry<sprite_vertex, uint32_t> geometry
    {
        .vertices
        {
            { position.min, { u_left, v_top } },
            { { position.max.x, position.min.y }, { u_right, v_top } },
            {  position.max, { u_right, v_bottom } },
            { { position.min.x, position.max.y }, { u_left, v_bottom } }
        },
        .elements
        {
            0, 2, 1,
            0, 3, 2
        }
    };

    return geometry;
}

static auto create_sprite(const sprite_pivot pivot, const float x, const float y, const float width, const float height,
                          const float atlas_width, const float atlas_height, const bool flip_vertical) -> geometry<sprite_vertex, uint32_t>
{
    switch (pivot)
    {
        case sprite_pivot::center:
        {
            const auto half_width = width / 2.0f;
            const auto half_height = height / 2.0f;

            return create_sprite({ -half_width, -half_height, half_width, half_height },
                x, y, width, height, atlas_width, atlas_height, flip_vertical);
        }
        default:
            return create_sprite({ 0.0f, 0.0f, width, height }, x, y,
                width, height, atlas_width, atlas_height, flip_vertical);
    }
}