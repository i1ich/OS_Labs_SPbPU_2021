cmake_minimum_required(VERSION 3.1)
project(ipc_Kirpichenko)

set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
add_compile_options(-Wall -Werror)
find_package(Threads REQUIRED)

set(INTERACTION_TYPES "mmap" "shm" "pipe")

foreach(TYPE_CODE IN LISTS INTERACTION_TYPES)
    add_executable("host_${TYPE_CODE}" host.cpp host.h client.cpp client.h conn_interface.h "conn_${TYPE_CODE}.cpp" "conn_${TYPE_CODE}.h")
    target_compile_definitions("host_${TYPE_CODE}" PRIVATE TYPE_CODE=${TYPE_CODE})
    target_link_libraries("host_${TYPE_CODE}" PRIVATE Threads::Threads rt)
endforeach()
