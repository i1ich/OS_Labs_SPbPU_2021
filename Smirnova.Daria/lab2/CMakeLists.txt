cmake_minimum_required(VERSION 3.19)
project(lab1_var16)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

find_package (Threads)
find_library (LIBRT rt)

add_executable(host_fifo utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_fifo.cpp
        host/GameHost.cpp host/GameHost.h host/host.cpp)
add_executable(client_fifo utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_fifo.cpp
        client/client.cpp client/Goat.h client/Goat.cpp)
target_link_libraries(host_fifo PUBLIC pthread lrt)
target_link_libraries(client_fifo PUBLIC pthread lrt)

add_executable(host_mq utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_mq.cpp
        host/GameHost.cpp host/GameHost.h host/host.cpp)
add_executable(client_mq utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_mq.cpp
        client/client.cpp client/Goat.h client/Goat.cpp)
target_link_libraries(host_mq PUBLIC pthread rt)
target_link_libraries(client_mq PUBLIC pthread rt)

add_executable(host_seg utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_seg.cpp
        host/GameHost.cpp host/GameHost.h host/host.cpp)
add_executable(client_seg utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_seg.cpp
        client/client.cpp client/Goat.h client/Goat.cpp)
target_link_libraries(host_seg PUBLIC pthread)
target_link_libraries(client_seg PUBLIC pthread)
