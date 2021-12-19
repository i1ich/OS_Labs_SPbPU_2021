cmake_minimum_required(VERSION 3.15)
project(lab2)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

find_package (Threads)
find_library (LIBRT rt)

file(GLOB CONNECTIONS connection/conn_*.cpp)
file(GLOB HOST host/*.cpp host/*.h)
message(${HOST})
foreach(CONN ${CONNECTIONS})

        get_filename_component(FILE_NAME ${CONN} NAME_WLE )
        string(REGEX REPLACE "conn_" "" CONN_NAME ${FILE_NAME})
        
        add_executable(host_${CONN_NAME} ${HOST}
                connection/connection.h connection/conn_${CONN_NAME}.cpp connection/conn_${CONN_NAME}.h)

        target_link_libraries(host_${CONN_NAME} PUBLIC pthread rt)
endforeach()
