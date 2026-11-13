#include "sprite_packer.hpp"
#include "resources.hpp"
#include "geometry_utils.hpp"

auto SpritePacker::init(const std::string_view folder, const uint32_t width, const uint32_t height) -> void
{
    images_data = load_textures(folder);

    stbrp_context stbrp_context;

    std::vector<stbrp_node> atlas_nodes(width);

    stbrp_init_target(&stbrp_context, static_cast<int32_t>(width), static_cast<int32_t>(height), atlas_nodes.data(), static_cast<int32_t>(atlas_nodes.size()));

    const auto rectangles_count = static_cast<int32_t>(images_data.size());
    rectangles.resize(rectangles_count);

    constexpr auto atlas_padding = 2;

    for (auto i = 0; i < images_data.size(); i++)
    {
        const auto& image_data = images_data[i];

        rectangles[i].w = image_data.width  + atlas_padding;
        rectangles[i].h = image_data.height + atlas_padding;
    }

    const auto all_packed = stbrp_pack_rects(&stbrp_context, rectangles.data(), rectangles_count);
    printf("Packing status: %s\n\n", all_packed ? "All packed successfully!" : "Some rectangles did not fit.");

    for (auto& rectangle : rectangles)
    {
        rectangle.x += atlas_padding / 2;
        rectangle.y += atlas_padding / 2;
        rectangle.w -= atlas_padding;
        rectangle.h -= atlas_padding;
    }

    std::ofstream output_file;
    output_file.open(std::string(folder) + ".txt", std::ios::trunc);
    // TODO check what this ios::trunc does
    // TODO have the extension as sprites

    for (auto i = 0; i < rectangles_count; ++i)
    {
        if (rectangles[i].was_packed)
        {
            // TODO move this sprite atlas write into a separate function/class
            // TODO find a better/best way to write this in a file
            output_file << rectangles[i].x << " " << rectangles[i].y << " " << rectangles[i].w << " " << rectangles[i].h << std::endl;

            rectangles_geometry.append(create_sprite({ 0, 0,  static_cast<float>(rectangles[i].w), static_cast<float>(rectangles[i].h) }, false));
        }
        else
        {
            printf("Rect FAILED to pack [Size: %dx%d]\n", rectangles[i].w, rectangles[i].h);
        }
    }
}

auto SpritePacker::build() -> void
{
}
