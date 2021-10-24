cmake_minimum_required(VERSION 3.0)
project(Daemon)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED PID_DIRECTORY)
    message(FATAL_ERROR "Define PID_DIRECTORY")
endif()

add_compile_options(-Wall -Werror)
add_definitions(-DPID_DIRECTORY="${PID_DIRECTORY}")

file(GLOB SOURCE_FILES *.cpp *.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})