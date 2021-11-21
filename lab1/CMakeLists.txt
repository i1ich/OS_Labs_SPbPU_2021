cmake_minimum_required(VERSION 3.18)
project(lab1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

add_executable(lab1 "main.cpp" "daemon.h" "daemon.cpp" )
target_link_libraries(lab1 stdc++fs)
