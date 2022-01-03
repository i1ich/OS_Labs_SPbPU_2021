cmake_minimum_required(VERSION 3.0.0)
project(lock_free_struct VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SRC "src/*.cpp")

add_compile_options(-Wall -Werror)

add_executable(lock_free_struct ${SRC})

target_link_libraries(${PROJECT_NAME} rt pthread)