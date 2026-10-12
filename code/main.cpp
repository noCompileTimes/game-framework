#include <glad/glad.h>

#include <print>

#include "backends/glfw/window_factory.hpp"

#include "core/window_manager.hpp"

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

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    while (is_closing == false)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        window_manager.update();
    }

    window->destroy();

    return 0;
}