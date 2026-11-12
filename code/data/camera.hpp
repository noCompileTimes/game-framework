#pragma once

namespace data
{
    struct alignas(16) camera
    {
        glm::mat4 view;
        glm::mat4 projection;
    };
}