#pragma once

#include "texture_data.hpp"

static auto load_texture(const std::string_view filename) -> texture_data
{
    texture_data data { };

    data.pixels = stbi_load(filename.data(), &data.width, &data.height, &data.channels, 0);

    if (data.pixels == nullptr)
    {
        printf("Failed to load texture '%s': %s\n", filename.data(), stbi_failure_reason());
    }

    return data;
}

static auto load_textures(const std::vector<std::string_view>& filenames) -> std::vector<texture_data>
{
    std::vector<texture_data> data;

    for (const auto& filename : filenames)
    {
        data.emplace_back(load_texture(filename));
    }

    return data;
}

static auto load_textures(const std::string_view folder) -> std::vector<texture_data>
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

    std::vector<texture_data> data;

    for (const auto& filename : filenames)
    {
        data.emplace_back(load_texture(filename.string()));
    }

    return data;
}