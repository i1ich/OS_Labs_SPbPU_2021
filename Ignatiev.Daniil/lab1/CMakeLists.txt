cmake_minimum_required(VERSION 3.15)
project(Daemon)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(Daemon main.cpp daemon.h daemon.cpp parser.h parser.cpp )
