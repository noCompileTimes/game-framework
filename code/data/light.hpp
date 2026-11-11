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

// TODO decide if this data namespace is enough or we need to put it under core when it will be in a module?
// TODO think on the item above - do this for all the data folder members