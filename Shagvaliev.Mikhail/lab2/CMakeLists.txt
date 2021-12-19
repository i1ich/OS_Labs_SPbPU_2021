cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror)

file(GLOB CONNS conn/conn_*.h)
file(GLOB HOST host/*.cpp host/*.h)
file(GLOB CLIENT client/*.cpp client/*.h)
file(GLOB UTILS utils/*.h)
file(GLOB MSG conn/message.h)

foreach(CONN_PATH ${CONNS})
    get_filename_component(FILE_NAME ${CONN_PATH} NAME_WLE)
    string(REGEX REPLACE "conn_" "" TYPE_MODE ${FILE_NAME})

    add_executable(client_${TYPE_MODE} ${MSG} ${CLIENT} ${UTILS} conn/conn.h
            conn/conn_${TYPE_MODE}.cpp conn/conn_${TYPE_MODE}.h)

    add_executable(host_${TYPE_MODE} ${MSG} ${HOST} ${UTILS} conn/conn.h
            conn/conn_${TYPE_MODE}.cpp conn/conn_${TYPE_MODE}.h)

    target_link_libraries(client_${TYPE_MODE} rt pthread)
    target_link_libraries(host_${TYPE_MODE} rt pthread)
endforeach()