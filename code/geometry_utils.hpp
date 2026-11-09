#pragma once

#include "core/rect.hpp"

#include "geometry.hpp"

// TODO add a new version of create sprite which takes an x, y, w, h and some size for the texture?

// TODO check if this flip_vertical parameter it makes what stbi_set_flip_vertically_on_load does for stb image on load
// flip_vertical = true  -> for textures stored bottom-up (e.g. framebuffer / render target textures)
// flip_vertical = false -> for textures loaded with stb_image (stored top-down)
static auto create_sprite(const core::rect& position, const bool flip_vertical) -> geometry<sprite_vertex, uint32_t>
{
    const auto v_top    = flip_vertical ? 1.0f : 0.0f;
    const auto v_bottom = flip_vertical ? 0.0f : 1.0f;

    geometry<sprite_vertex, uint32_t> geometry
    {
        {
            { position.min, { 0.0f, v_top } },
            { { position.max.x, position.min.y }, { 1.0f, v_top } },
            {  position.max, { 1.0f, v_bottom } },
            { { position.min.x, position.max.y }, { 0.0f, v_bottom } }
        },
        {
            0, 2, 1,
            0, 3, 2
        }
    };

    return geometry;
}

static auto create_rectangle(const core::rect& position, const core::rgb& color) -> geometry<editor_vertex, uint32_t>
{
    geometry<editor_vertex, uint32_t> geometry
    {
        // TODO adding the 0.5f padding in here? makes sense? check with the claude about this 0.5f to be sure
        {
            { { position.min.x, position.min.y, 0.0f }, color },
            { { position.max.x, position.min.y, 0.0f }, color },
            { { position.max.x, position.max.y, 0.0f }, color },
            { { position.min.x, position.max.y, 0.0f }, color }

        },
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0
        }
    };

    return geometry;
}