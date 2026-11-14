#pragma once

struct texture_data // TODO maybe this can be called image_data or image with a data namespace
{
    int32_t  width;
    int32_t  height;
    int32_t  channels;

    uint8_t* pixels;
};