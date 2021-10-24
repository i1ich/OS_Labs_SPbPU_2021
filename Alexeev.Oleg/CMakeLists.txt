cmake_minimum_required(VERSION 3.20)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(lab1 src/main.cpp src/Daemon.cpp includes/Daemon.h src/Parser.cpp includes/Parser.h)
