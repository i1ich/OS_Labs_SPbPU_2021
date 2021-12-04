cmake_minimum_required(VERSION 3.16)
project(lab1_var16)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

find_package (Threads)
find_library (LIBRT rt)

add_executable(host_mq utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_mq.cpp connections/conn_mq.h
        host/GameHost.cpp host/GameHost.h host/host.cpp)
add_executable(client_mq utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_mq.cpp connections/conn_mq.h
        client/client.cpp client/Goat.h client/Goat.cpp)
target_link_libraries(host_mq PUBLIC pthread rt)
target_link_libraries(client_mq PUBLIC pthread rt)

add_executable(host_seg utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_seg.cpp connections/conn_seg.h
        host/GameHost.cpp host/GameHost.h host/host.cpp)
add_executable(client_seg utils/Message.h utils/ClientInfo.h utils/GameRules.h
        connections/Connection.h connections/conn_seg.cpp connections/conn_seg.h
        client/client.cpp client/Goat.h client/Goat.cpp)
target_link_libraries(host_seg PUBLIC pthread)
target_link_libraries(client_seg PUBLIC pthread)
