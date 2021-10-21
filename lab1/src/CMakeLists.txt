project(lab_daemon)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Werror)

add_executable(${CMAKE_PROJECT_NAME} main.cpp 
                Daemon.cpp
                Daemon.h
                DaemonManager.cpp
                DaemonManager.h
                )

