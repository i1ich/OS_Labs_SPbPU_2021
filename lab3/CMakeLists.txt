cmake_minimum_required(VERSION 3.17)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)
add_compile_definitions(_DEBUG)

add_executable(lab3 "main.cpp" "test.h" "test.cpp" "queue.h" "queue.cpp")
target_link_libraries(lab3 PUBLIC pthread)