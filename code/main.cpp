#include <glad/glad.h>
#include <glm/glm.hpp>

#include <print>

#include "backends/glfw/window_factory.hpp"

#include "core/window_manager.hpp"

#include "graphics/opengl/buffer.hpp"

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

static const vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
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

    graphics::opengl::Buffer geometry_vbo_new;
    geometry_vbo_new.create();
    geometry_vbo_new.bind(GL_ARRAY_BUFFER);
    geometry_vbo_new.data(GL_ARRAY_BUFFER, vertices, sizeof(vertices));

    uint32_t geometry_vao;
    glCreateVertexArrays(1, &geometry_vao);
    glBindVertexArray(geometry_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while (is_closing == false)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(geometry_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window_manager.update();
    }

    glDeleteVertexArrays(1, &geometry_vao);

    geometry_vbo_new.destroy();

    window->destroy();

    return 0;
}