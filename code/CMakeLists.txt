           add_executable(${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/headers.hpp>) # TODO remove the $<$<COMPILE_LANGUAGE:CXX>:...> once we do not build any c libraries anymore (it only stops a c precompiled header from being generated)
    target_link_libraries(${PROJECT_NAME} PRIVATE graphics-module platforms-module mathematics-module physics-module utilities-module)
    target_link_libraries(${PROJECT_NAME} PRIVATE assimp freetype glm stb) # imgui

           target_sources(${PROJECT_NAME} PRIVATE main.cpp sprite_packer.cpp)