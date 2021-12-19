cmake_minimum_required(VERSION 3.18.4)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)

set(CLIENT client)
set(HOST host)
set(CONNECTION conn)
set(DATA data)

add_compile_options(-Wall -Werror)

include_directories(${CONNECTION})
include_directories(${DATA})

foreach(CONN mq fifo socket)
    add_executable(host_${CONN} ${HOST}/wolf.cpp ${HOST}/wolf.h ${HOST}/host.cpp
            ${CLIENT}/goat.cpp ${CLIENT}/goat.h
            ${DATA}/common_data.h
            ${CONNECTION}/conn.h ${CONNECTION}/conn_${CONN}.cpp ${CONNECTION}/message.h)

    target_link_libraries(host_${CONN} rt pthread)
endforeach(CONN mq fifo socket)
