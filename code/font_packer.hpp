#pragma once

#include "geometry.hpp"
#include "vertex_types.hpp"
#include "image_data.hpp"

#include "graphics/opengl/texture.hpp"
#include "graphics/opengl/shader_program.hpp"
#include "graphics/opengl/texture_sampler.hpp"
#include "graphics/opengl/buffer.hpp"

// TODO maybe call it SpriteAtlasPacker? - check the english
// TODO maybe at some point this can be extended from a tool class
class FontPacker
{
public:
    FontPacker(const std::string& folder, uint32_t width, uint32_t height);

    auto  init(const std::vector<image_data>& images) -> void;
    auto build(const graphics::opengl::ShaderProgram&  shader, // TODO at some point use a resource/graphics manager to query for this shaders/samplers/uniform buffers
               const graphics::opengl::TextureSampler& sampler,
               const graphics::opengl::Buffer& camera_ubo,
               const graphics::opengl::Buffer& transform_ubo) -> void;

    // top of the atlas is at v = 1 (flipped compared to the png/txt) - see the uv comment in init()
    graphics::opengl::Texture texture; // TODO when we will have a resource/graphics manager, this should be private

private:
    auto pack() -> void; // TODO maybe call this pack or packing? because it kind of does that

    geometry<sprite_vertex, uint32_t> _geometry;

    std::vector<image_data> _images;
    std::vector<stbrp_rect> _rectangles;

    std::string _folder;

    uint32_t _width;
    uint32_t _height;
};

// TODO maybe the drawing can be in another class? like SpriteAtlasPackerDrawer? think of it

// TODO lets be sure always that width equals height - we always want square textures - better for performance
// TODO maybe if we can also have them as power of two? - hope I used the right terms