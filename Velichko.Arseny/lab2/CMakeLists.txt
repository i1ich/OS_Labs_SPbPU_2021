cmake_minimum_required(VERSION 3.10)
project(IPC)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

add_subdirectory(Common)
add_subdirectory(Utility)
add_subdirectory(Connections)
add_subdirectory(Host)
add_subdirectory(Client)
