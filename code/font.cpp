#include "font.hpp"

#include "graphics/opengl/pipeline.hpp"

auto Font::load(const std::string& filename) -> int32_t
{
    FT_Library ft_library;

    if (FT_Init_FreeType(&ft_library))
    {
        return -1;
    }

    FT_Face ft_face;

    if (FT_New_Face(ft_library, filename.data(), 0, &ft_face))
    {
        return -1;
    }

    FT_Set_Pixel_Sizes(ft_face, 0, 48);

    ascender = static_cast<int32_t>(ft_face->size->metrics.ascender >> 6); // 26.6 fixed point to pixels

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        // FT_Load_Char does not fail for characters the font does not have, it silently loads glyph 0 (.notdef, the "tofu" box)
        // FT_Get_Char_Index returns 0 for those, so we skip them (e.g. the ascii control characters 0 - 31 and 127 in most fonts)
        const auto glyph_index = FT_Get_Char_Index(ft_face, c);

        if (glyph_index == 0)
        {
            continue;
        }

        if (FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_RENDER)) // real errors only (broken font, render failure)
        {
            std::println("Failed to load glyph {}", static_cast<int32_t>(c));

            continue;
        }

        graphics::opengl::Texture texture;
        texture.type = GL_TEXTURE_2D;
        texture.create();
        texture.storage(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows, GL_R8, 1); // TODO can we use direclty the alpha here?
        texture.subdata(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows, GL_RED, 0, ft_face->glyph->bitmap.buffer);

        text_character character;
        character.texture = texture;
        character.size = glm::ivec2(ft_face->glyph->bitmap.width, ft_face->glyph->bitmap.rows);
        character.bearing = glm::ivec2(ft_face->glyph->bitmap_left, ft_face->glyph->bitmap_top);
        character.advance = static_cast<unsigned int>(ft_face->glyph->advance.x);

        characters.insert(std::make_pair(c, character));

        const auto& bitmap = ft_face->glyph->bitmap;
        const auto width  = static_cast<int32_t>(bitmap.width);
        const auto height = static_cast<int32_t>(bitmap.rows);

        // copy the glyph out of FreeType's glyph slot: bitmap.buffer is overwritten by the next FT_Load_Char and freed by FT_Done_Face
        // malloc so it is released like the other image_data pixels (SpritePacker frees them with stbi_image_free, which is free)
        auto* pixels = static_cast<uint8_t*>(std::malloc(static_cast<size_t>(width) * static_cast<size_t>(height)));

        if (pixels != nullptr)
        {
            for (auto row = 0; row < height; row++) // row by row: FreeType rows are `pitch` bytes apart, which can be more than width
            {
                std::memcpy(pixels + static_cast<size_t>(row) * width, bitmap.buffer + static_cast<ptrdiff_t>(row) * bitmap.pitch, width);
            }
        }

        images.emplace_back(image_data { width, height, 1, pixels });
    }

    graphics::opengl::Pipeline::pixel_store(GL_UNPACK_ALIGNMENT, 4);

    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);

    return 0;
}