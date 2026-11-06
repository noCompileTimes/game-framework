#pragma once

// TODO - chekc how to align it for 16bits to be usefull in the shader for std140?
struct light
{
    glm::vec3 color;
    float temp1; // TODO maybe not correct
    glm::vec3 source;
    float temp2; // TODO maybe not correct
};