#pragma once

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