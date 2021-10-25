cmake_minimum_required(VERSION 3.15)
project(daemon_lab)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

add_executable(daemon_lab main.cpp Daemon.cpp Daemon.h Parser.cpp Parser.h)
