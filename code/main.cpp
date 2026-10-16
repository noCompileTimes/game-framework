#include <glad/glad.h>
#include <glm/glm.hpp>

#include <print>

#include "backends/glfw/window_factory.hpp"

#include "core/window_manager.hpp"

#include "graphics/opengl/buffer.hpp"
#include "graphics/opengl/vertex_array.hpp"
#include "graphics/opengl/commands.hpp"

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

struct vertex
{
    glm::vec2 position;
    glm::vec3 color;
};

static const vertex geometry_vertices[3]
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

static const uint32_t geometry_elements[3]
{
    0, 1, 2
};

auto main() -> int32_t
{
    const std::unique_ptr<core::base::WindowFactory> window_factory = std::make_unique<backends::glfw::WindowFactory>();

    core::WindowManager window_manager;
    window_manager.init(window_factory);

    const auto window = window_manager.window();
    const auto window_events = window_manager.window_events();

    auto is_closing = false;

    window_events->callbacks.on_close = [&]
    {
        is_closing = true;
    };

    window_events->callbacks.on_resize = [](uint32_t width, uint32_t height)
    {
        std::println("resize {} {}", width, height);
    };

    gladLoadGL();

    graphics::opengl::Buffer geometry_vbo;
    geometry_vbo.create();
    geometry_vbo.bind(GL_ARRAY_BUFFER); // TODO remove this
    geometry_vbo.data(sizeof(geometry_vertices), geometry_vertices);

    graphics::opengl::Buffer geometry_ebo;
    geometry_ebo.create();
    geometry_ebo.bind(GL_ELEMENT_ARRAY_BUFFER); // TODO remove this
    geometry_ebo.data(sizeof(geometry_elements), geometry_elements);

    graphics::opengl::VertexArray geometry_vao;
    geometry_vao.create();
    geometry_vao.bind();

    geometry_vao.attach_vertices(geometry_vbo, sizeof(vertex));
    geometry_vao.attach_elements(geometry_ebo);

    geometry_vao.attach_attribute({ 0, offsetof(vertex, position), 2, GL_FLOAT, sizeof(vertex) });
    geometry_vao.attach_attribute({ 1, offsetof(vertex, color), 3, GL_FLOAT, sizeof(vertex) });

    graphics::opengl::Commands::clear(0.5f, 0.5f, 0.5f, 1.0f);

    while (is_closing == false)
    {
        graphics::opengl::Commands::clear(GL_COLOR_BUFFER_BIT);

        geometry_vao.bind();

        graphics::opengl::Commands::draw_elements(GL_TRIANGLES, 0, 3);

        window_manager.update();
    }

    geometry_vao.destroy();
    geometry_ebo.destroy();

    geometry_vbo.destroy();

    window->destroy();

    return 0;
}