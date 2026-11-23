#pragma once

namespace data
{
    // the camera position for the lighting (view_ubo in model_frag_shader), in its own buffer:
    // a bind range inside camera_ubo would need its offset to be a multiple of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT (256 here)
    struct alignas(16) view
    {
        glm::vec3 position;
    };
}