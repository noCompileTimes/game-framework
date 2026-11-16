#pragma once

enum class sprite_pivot : uint32_t
{
    top_left,
    top_right,

    bottom_left,
    bottom_right,

    center,
    custom // TODO if this is applied in general you should affect directly the rect position in the create_sprite function
};