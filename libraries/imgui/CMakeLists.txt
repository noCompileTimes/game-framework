project(imgui)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC . backends)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw opengl32)

target_sources(${PROJECT_NAME} PUBLIC imgui.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp
        backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp)