project(Structures)

find_package(Threads REQUIRED)

add_library(Structures INTERFACE)
target_include_directories(Structures INTERFACE .)
target_link_libraries(Structures INTERFACE Threads::Threads)