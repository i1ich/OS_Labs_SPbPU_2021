cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++)
add_compile_options(-Wall -Werror)

set(HOST_SRC host)
set(CLIENT_SRC client)
set(CONN conn)
set(SETTINGS settings)

separate_arguments(CONN_TYPES)

foreach(TYPE_MODE ${CONN_TYPES})
    message(${TYPE_MODE})
    add_executable(client_${TYPE_MODE} ${CLIENT_SRC}/client.cpp ${CLIENT_SRC}/goat.cpp ${CLIENT_SRC}/goat.h
            ${CONN}/conn.h ${CONN}/${CONN}_${TYPE_MODE}.cpp ${CONN}/${CONN}_${TYPE_MODE}.h
            ${SETTINGS}/GameSettings.h)

    add_executable(host_${TYPE_MODE} ${HOST_SRC}/host.cpp ${HOST_SRC}/wolf.cpp ${HOST_SRC}/wolf.h
            ${CONN}/conn.h ${CONN}/${CONN}_${TYPE_MODE}.cpp ${CONN}/${CONN}_${TYPE_MODE}.h
            ${SETTINGS}/GameSettings.h)

    target_link_libraries("client_${TYPE_MODE}" rt pthread)
    target_link_libraries("host_${TYPE_MODE}" rt pthread)

endforeach()