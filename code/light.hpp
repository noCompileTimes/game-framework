#pragma once

// TODO - chekc how to align it for 16bits to be usefull in the shader for std140?
struct light
{
    glm::vec3 color;
    float intensity;

    glm::vec3 source; // directional light: the direction it travels
                      // point light: its position
    float temp;
};