cmake_minimum_required(VERSION 3.20)
project(Daemon)

set(CMAKE_CXX_STANDARD 14)

add_executable(Daemon main.cpp daemon.h daemon.cpp parser.h parser.cpp )
