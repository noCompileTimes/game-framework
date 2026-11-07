add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PRIVATE platforms-module graphics-module mathematics-module utilities-module)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp freetype glfw glad glm stb BulletCollision LinearMath) # BulletDynamics imgui

target_sources(${PROJECT_NAME} PRIVATE main.cpp)