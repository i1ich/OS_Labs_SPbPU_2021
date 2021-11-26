cmake_minimum_required(VERSION 3.20)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab2 main.cpp client/client.cpp client/client.h host/host.cpp host/host.h conn/IConn.h data/WeatherDTO.cpp data/WeatherDTO.h conn/Conn_mq.cpp conn/Conn_seg.cpp conn/Conn_fifo.cpp)
