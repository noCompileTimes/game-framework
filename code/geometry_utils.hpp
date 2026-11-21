#pragma once

#include "core/aabb.hpp"
#include "core/rect.hpp"

#include "vertex_types.hpp"

#include "geometry.hpp"

[[nodiscard, maybe_unused]]
static auto create_rectangle(const core::rect& bounds, const core::rgb& color) -> geometry<editor_vertex, uint32_t>
{
    constexpr auto default_z = 0.0f; // TODO maybe this can be a constant somewhere?

    return geometry<editor_vertex, uint32_t>
    {
        // TODO adding the 0.5f padding in here? makes sense? check with the claude about this 0.5f to be sure
        //      not sure why I added this comment, maybe for the physics? to not overlapp on the current sprite if it is on top for debug?
        .vertices
        {
            { { bounds.min.x, bounds.min.y, default_z }, color },
            { { bounds.max.x, bounds.min.y, default_z }, color },
            { { bounds.max.x, bounds.max.y, default_z }, color },
            { { bounds.min.x, bounds.max.y, default_z }, color }

        },
        .elements
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0
        }
    };
}

[[nodiscard, maybe_unused]]
static auto create_cube(const core::aabb& bounds) -> geometry<editor_vertex, uint32_t>
{
    return geometry<editor_vertex, uint32_t>
    {
        .vertices
        {
        },
        .elements
        {
        }
    };
}

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