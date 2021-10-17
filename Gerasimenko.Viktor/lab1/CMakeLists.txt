cmake_minimum_required(VERSION 3.15)
project(lab_1)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(lab_1 main.cpp definitions.h parser.cpp parser.h daemon.cpp daemon.h)
