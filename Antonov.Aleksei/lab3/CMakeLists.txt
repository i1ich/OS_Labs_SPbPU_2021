cmake_minimum_required(VERSION 3.16)
project(AsyncStructure)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SRC src/*.cpp src/*.h)
file(GLOB TEST test/*.cpp test/*.h)
include_directories(Include)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_executable(${PROJECT_NAME} ${SRC} ${TEST})
target_link_libraries(${PROJECT_NAME} PUBLIC pthread)