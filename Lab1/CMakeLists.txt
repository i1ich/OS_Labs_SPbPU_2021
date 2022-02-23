cmake_minimum_required(VERSION 3.19)
project(lab1_16)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

add_executable(lab1_16 main.cpp Daemon.cpp Daemon.h utils.cpp)
