cmake_minimum_required(VERSION 3.17)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1 src/main.cpp include/Parser.h include/definitions.h include/Logger.h src/Parser.cpp src/Daemon.cpp include/Daemon.h include/DirectoryWithIgnFile.h)