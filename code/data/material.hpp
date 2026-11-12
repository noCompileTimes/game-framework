#pragma once

#include "core/rgb.hpp"

namespace data
{
    struct alignas(16) material
    {
        core::rgb albedo_color;
    };
}