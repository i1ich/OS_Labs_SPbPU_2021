cmake_minimum_required(VERSION 3.10)
project(lab3)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)
add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

add_subdirectory(Utility)
add_subdirectory(Structures)
add_subdirectory(Tests)