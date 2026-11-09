#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "vertex_types.hpp"

template <typename Vertex>
concept vertex_type = std::is_trivially_copyable_v<Vertex> && std::is_standard_layout_v<Vertex>;

template <typename Element>
concept element_type = std::same_as<Element, uint16_t> || std::same_as<Element, uint32_t>;

template <vertex_type Vertex, element_type Element>
struct geometry
{
    std::vector<Vertex>  vertices;
    std::vector<Element> elements;

    // TODO add some functions to be able to expend the geometry easy, something like an append

    auto append(const geometry& other) -> void
    {
        const auto offset = static_cast<Element>(vertices.size());

        vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());

        for (const auto element : other.elements)
        {
            elements.emplace_back(static_cast<Element>(element + offset));
        }
    }

    // TODO - maybe add a reserve method also

    auto clear() -> void
    {
        vertices.clear();
        elements.clear();
    }
};

static auto load_geometry(const std::string_view filename) -> geometry<model_vertex, uint32_t>
{
    Assimp::Importer importer;

    if (const auto scene = importer.ReadFile(filename.data(), 0); scene != nullptr)
    {
        geometry<model_vertex, uint32_t> geometry;

        const auto mesh = scene->mMeshes[0];

        for (auto i = 0; i < mesh->mNumVertices; i++)
        {
            const auto& position = mesh->mVertices[i];
            const auto& normal = mesh->mNormals[i];
            const auto& texcoord = mesh->mTextureCoords[0][i];

            geometry.vertices.emplace_back(model_vertex { { position.x, position.y, position.z },  { normal.x, normal.y, normal.z }, { texcoord.x, texcoord.y } });
        }

        for (auto i = 0; i < mesh->mNumFaces; i++)
        {
            const auto& face = mesh->mFaces[i];

            for (auto j = 0; j < face.mNumIndices; j++)
            {
                geometry.elements.emplace_back(face.mIndices[j]);
            }
        }

        return geometry;
    }

    return { };
}