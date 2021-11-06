cmake_minimum_required(VERSION 3.12.0)
project(daemon VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

file(GLOB src "src/*.cpp")

add_compile_options(-Wall -Werror)

add_executable(daemon ${src})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)