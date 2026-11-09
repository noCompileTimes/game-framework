add_executable(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE graphics-module platforms-module mathematics-module physics-module utilities-module)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp freetype glm stb) # imgui

target_sources(${PROJECT_NAME} PRIVATE main.cpp)