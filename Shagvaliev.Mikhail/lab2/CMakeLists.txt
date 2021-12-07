cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror)

set(HOST_SRC host)
set(CLIENT_SRC client)
set(conn conn)
set(utils utils)

foreach(TYPE_MODE mq sock fifo)
    add_executable(client_${TYPE_MODE} ${CLIENT_SRC}/client.cpp ${CLIENT_SRC}/weather_predictor.cpp ${CLIENT_SRC}/weather_predictor.h
            ${CONN}/conn.h ${CONN}/${CONN}_${TYPE_MODE}.cpp
            ${UTILS}/definitions.h ${UTILS}/definitions.h)

    add_executable(host_${TYPE_MODE} ${HOST_SRC}/host.cpp ${HOST_SRC}/requestor.cpp ${HOST_SRC}/requestor.h
            ${CONN}/conn.h ${CONN}/${CONN}_${TYPE_MODE}.cpp
            ${UTILS}/definitions.h ${UTILS}/definitions.h)

    target_link_libraries(client_${TYPE_MODE} rt pthread)
    target_link_libraries(host_${TYPE_MODE} rt pthread)

endforeach()