cmake_minimum_required(VERSION 3.0.0)
project(mipc VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC "src/*.cpp")

add_compile_options(-Wall -Werror)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} rt pthread) 