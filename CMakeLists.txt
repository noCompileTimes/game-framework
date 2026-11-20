cmake_minimum_required(VERSION 4.3.1)

project(game-framework LANGUAGES CXX)

add_executable(${PROJECT_NAME})

add_subdirectory(tutorials)
add_subdirectory(libraries)
add_subdirectory(modules)
add_subdirectory(code)