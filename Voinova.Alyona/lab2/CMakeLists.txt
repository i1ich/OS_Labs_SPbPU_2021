cmake_minimum_required(VERSION 3.15)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

find_package (Threads)
find_library (LIBRT rt)

file(GLOB CONNECTIONS conn/conn_*.cpp)
file(GLOB HOST host/*.cpp host/*.h)
file(GLOB CLIENT client/*.cpp client/*.h)
file(GLOB DTO data/*.h data/*.cpp)
file(GLOB STUFF stuff/*.h stuff/*.cpp)

foreach(PATH_TO_CON ${CONNECTIONS})
        get_filename_component(FILE_NAME ${PATH_TO_CON} NAME_WLE )
        string(REGEX REPLACE "conn_" "" CONN_NAME ${FILE_NAME})

        add_executable(host_${CONN_NAME} ${DTO} ${STUFF} ${HOST}
                conn/IConn.h conn/conn_${CONN_NAME}.cpp)
        add_executable(client_${CONN_NAME} ${DTO} ${STUFF} ${CLIENT}
                conn/IConn.h  conn/conn_${CONN_NAME}.cpp)
        target_link_libraries(host_${CONN_NAME} PUBLIC pthread rt)
        target_link_libraries(client_${CONN_NAME} PUBLIC pthread rt)
endforeach()
