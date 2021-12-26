cmake_minimum_required(VERSION 3.16)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror)

file(GLOB SRC src/*.h src/*.cpp)
file(GLOB TEST test/*.h test/*.cpp)
file(GLOB UTILS util/*.h util/*.cpp)

add_executable(lab3 ${SRC} ${TEST} ${UTILS})

target_link_libraries(lab3 rt pthread)
