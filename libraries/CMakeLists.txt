add_subdirectory(glfw)
add_subdirectory(glad)

# assimp is used as a submodule, so it should not install/export its targets (they link our stb target, which is not exported)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)
add_subdirectory(stb)

add_subdirectory(bullet)
add_subdirectory(glm)

add_subdirectory(imgui)
add_subdirectory(imgui/examples)

add_subdirectory(imguizmo)

add_subdirectory(behaviortree-cpp)
add_subdirectory(recastnavigation)
add_subdirectory(ozz-animation)

add_subdirectory(freetype)
add_subdirectory(cereal)

add_subdirectory(SDL)