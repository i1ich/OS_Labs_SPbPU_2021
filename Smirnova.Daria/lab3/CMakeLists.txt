cmake_minimum_required(VERSION 3.16)
project(lab3_var2)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

file(GLOB CONTAINERS src/*.h)
file(GLOB TESTS test/*.cpp test/*.h)

find_package (Threads)

add_executable(lab3_var2 ${TESTS} ${CONTAINERS})
target_link_libraries(lab3_var2 PUBLIC pthread)
