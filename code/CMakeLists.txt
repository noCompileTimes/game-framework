add_executable(${PROJECT_NAME})

#target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src)

#target_link_libraries(${PROJECT_NAME} PRIVATE imgui BulletDynamics BulletCollision LinearMath)
target_link_libraries(${PROJECT_NAME} PRIVATE platforms-module graphics-module utilities-module assimp glfw glad glm stb)

target_sources(${PROJECT_NAME} PRIVATE main.cpp)