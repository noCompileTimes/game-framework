#pragma once

namespace data
{
    struct alignas(16) camera
    {
        glm::mat4 view;
        glm::mat4 projection;
    };
}

// TODO maybe the data structures can go into the graphics? because they are mostly for the shaders (ubo)