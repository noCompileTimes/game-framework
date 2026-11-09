#pragma once

#include "core/vec3.hpp"
#include "core/rgb.hpp"

namespace data
{
    struct alignas(16) light
    {
        core::rgb color;
        float intensity;

        core::vec3 source; // directional light: the direction it travels
                           // point light: its position
    };
}