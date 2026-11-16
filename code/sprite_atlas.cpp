#include "sprite_atlas.hpp"
#include "geometry.hpp"
#include "geometry_utils.hpp"
#include "vertex_types.hpp"
#include "sprite.hpp"
#include "resources.hpp"

// TODO refactor/improve it at some point

// TODO maybe a better name? read/load/etc
auto SpriteAtlas::init(std::string_view filename) -> void
{
    std::ifstream sprite_atlas_file { filename.data() };

    std::string texture_filename;
    sprite_atlas_file >> texture_filename;

    const auto sprite_atlas_image = load_image(texture_filename);

    uint32_t rectangles_count;
    sprite_atlas_file >> rectangles_count;

    std::vector<sprite> sprites;

    geometry<sprite_vertex, uint32_t> sprites_geometry;

    for (auto i = 0; i < rectangles_count; i++)
    {
        uint32_t pivot;  // TODO should this be part of the sprite struct?

        sprite sprite;
        sprite_atlas_file >> sprite.x >> sprite.y >> sprite.w >> sprite.h >> pivot;

        sprites.emplace_back(sprite);

        // TODO this should be gone when we have a way to not override this everytime we are starting the app
        // NOTE: this runs for every atlas, so an index here is centred in both characters and characters_faces
        //       characters: 3, 5 (blue), 41, 44 (red), 50, 59 (yellow) | characters_faces: 6, 10, 11
        if (i == 3 || i == 5 || i == 41 || i == 44 || i == 50 || i == 59 || i == 6 || i == 10 || i == 11)
        {
            pivot = static_cast<uint32_t>(sprite_pivot::center);
        }

        // TODO this can be made in another for loop after we are done with the reading? make sense?
        sprites_geometry.append(create_sprite(static_cast<sprite_pivot>(pivot), sprite.x, sprite.y, sprite.w, sprite.h,
            sprite_atlas_image.width, sprite_atlas_image.height, false));
    }

    graphics::opengl::Buffer sprites_vbo; // TODO keep a private reference to them, maybe destroy it at some point in a release function
    sprites_vbo.create();
    sprites_vbo.storage(sizeof(sprite_vertex) * sprites_geometry.vertices.size(), sprites_geometry.vertices.data(), 0);

    graphics::opengl::Buffer sprites_ebo; // TODO keep a private reference to them, maybe destroy it at some point in a release function
    sprites_ebo.create();
    sprites_ebo.storage(sizeof(uint32_t) * sprites_geometry.elements.size(), sprites_geometry.elements.data(), 0);

    _vao.create();

    _vao.attach_vertices(sprites_vbo, sizeof(sprite_vertex));
    _vao.attach_elements(sprites_ebo);

    _vao.attach_attribute({ 0, offsetof(sprite_vertex, position), 2, GL_FLOAT, 0 });
    _vao.attach_attribute({ 1, offsetof(sprite_vertex, texcoord), 2, GL_FLOAT, 0 });

    _texture.type = GL_TEXTURE_2D;
    _texture.create();
    _texture.storage(sprite_atlas_image.width, sprite_atlas_image.height, GL_RGBA8, 1);
    _texture.subdata(sprite_atlas_image.width, sprite_atlas_image.height, GL_RGBA, 0, sprite_atlas_image.pixels);
}

auto SpriteAtlas::bind() const -> void
{
    _texture.bind(0);
        _vao.bind();
}

// TODO create at some point some SpriteAtlasBatcher in which you can have just one vbo, ebo, vao which includes multiple
//      sprite atlases and we offset the indexes for the sprites - better from the performance perspective