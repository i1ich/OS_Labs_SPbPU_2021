cmake_minimum_required(VERSION 3.18)
project(lab1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall)

add_executable(lab1 main.cpp daemon.cpp "daemon.h")