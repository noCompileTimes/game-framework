#pragma once

#include "image_data.hpp"

inline auto load_image(const std::string_view filename) -> image_data
{
    image_data data { };

    data.pixels = stbi_load(filename.data(), &data.width, &data.height, &data.channels, 0);

    if (data.pixels == nullptr)
    {
        std::println("Failed to load texture '{}': {}", filename, stbi_failure_reason());
    }

    return data;
}

inline auto load_images(const std::vector<std::string_view>& filenames) -> std::vector<image_data>
{
    std::vector<image_data> data;

    for (const auto& filename : filenames)
    {
        data.emplace_back(load_image(filename));
    }

    return data;
}

inline auto load_textures(const std::string_view folder) -> std::vector<image_data>
{
    std::vector<std::filesystem::path> filenames;

    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        auto extension = entry.path().extension().string();
        std::ranges::transform(extension, extension.begin(), [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });

        // TODO move this supported extensions in a map/set somewhere where we can check on them in multiple places
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||extension == ".tga")
        {
            filenames.emplace_back(entry.path());
        }
    }

    std::ranges::sort(filenames);

    std::vector<image_data> data;

    for (const auto& filename : filenames)
    {
        data.emplace_back(load_image(filename.string()));
    }

    return data;
}