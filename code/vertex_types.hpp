#pragma once

#include "core/vec2.hpp"
#include "core/vec3.hpp"
#include "core/rgb.hpp"

struct model_vertex
{
    core::vec3 position;
    core::vec3 normal;
    core::vec2 texcoord;
};

struct sprite_vertex
{
    core::vec2 position;
    core::vec2 texcoord;
};

struct editor_vertex
{
    core::vec3 position;
    core::rgb  color;
};

struct text_vertex
{
    core::vec2 position;
    core::vec2 texcoord;
};