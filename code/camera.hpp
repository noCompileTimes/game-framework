#pragma once

#include "core/vec3.hpp"

#include <glm/glm.hpp>

inline auto screen_pos_to_world_ray(const int32_t mouse_x, const int32_t mouse_y, const int32_t screen_width,
                                    const int32_t screen_height, const glm::mat4& view, const glm::mat4& projection,
                                    core::vec3& out_origin, core::vec3& out_direction) -> void
{
    // mouse position (from the bottom-left corner) to normalized device coordinates [-1, 1]
    const auto ndc_x = (static_cast<float>(mouse_x) / static_cast<float>(screen_width)  - 0.5f) * 2.0f;
    const auto ndc_y = (static_cast<float>(mouse_y) / static_cast<float>(screen_height) - 0.5f) * 2.0f;

    // the near plane maps to z = -1, z = 0 is just another point further along the ray (only used for the direction)
    const glm::vec4 ray_start_ndc { ndc_x, ndc_y, -1.0f, 1.0f };
    const glm::vec4 ray_end_ndc   { ndc_x, ndc_y,  0.0f, 1.0f };

    // projection * view goes from world space to clip space, so its inverse goes from ndc back to world space
    const auto inverse_view_projection = glm::inverse(projection * view);

    auto ray_start_world = inverse_view_projection * ray_start_ndc;
         ray_start_world /= ray_start_world.w;

    auto ray_end_world = inverse_view_projection * ray_end_ndc;
         ray_end_world /= ray_end_world.w;

    const auto ray_direction_world = normalize(glm::vec3(ray_end_world - ray_start_world));

    out_origin    = { ray_start_world.x, ray_start_world.y, ray_start_world.z };
    out_direction = { ray_direction_world.x, ray_direction_world.y, ray_direction_world.z };
}

// TODO this should go into the physics module?
// if yes - we should return a ray structure for the origin and direction
//        - we should add mat4 into the math in order to not include glm in the physics module
// TODO optimize this function (example: cache the inverse view projection when the camera does not change)
// TODO this assumes the 3D scene fills the whole window, if we render it into a part of it (editor viewport, split screen)
//      we need to pass the viewport offset and size, subtract the offset from the mouse position and use the viewport size for the ndc conversion