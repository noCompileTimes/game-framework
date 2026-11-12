#pragma once

namespace data
{
    enum class binding : uint32_t
    {
        // vertex stage
        camera,
        transform,

        // fragment stage
        material,
        light
    };
}

// TODO check if there is possible to somehow put these values in the shaders as constants maybe? at compile time?
// TODO maybe the item above will work when we compile to sprv in the build stage there is some constants there?