cmake_minimum_required(VERSION 3.18.4)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror)

include_directories(host client conn data)

foreach(CONN mq fifo socket)
    add_executable(host_${CONN} host/wolf.cpp host/host.cpp
            client/goat.cpp client/goat.h
            data/common_data.h
            conn/conn.h conn/conn_${CONN}.cpp conn/message.h)

    target_link_libraries(host_${CONN} rt pthread)
endforeach(CONN mq fifo socket)
