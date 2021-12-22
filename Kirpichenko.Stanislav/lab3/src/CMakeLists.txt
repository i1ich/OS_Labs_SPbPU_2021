cmake_minimum_required(VERSION 3.1)
project(Kirpichenko_lab3)


set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
#add_compile_options(-Wall -Werror)
find_package(Threads REQUIRED)

add_executable(source main.cpp SCSPQueue.h SCSPQueue.tpp
LazySet.h LazySet.tpp MutexGuardLock.h MutexGuardLock.cpp
Tester.h QueueTester.h QueueTester.cpp SetTester.h SetTester.cpp 
TimeTester.h TimeTester.cpp)