project(Utility)

find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)

add_library(Utility ${SOURCE_FILES})
target_link_libraries(Utility PUBLIC Threads::Threads)
target_include_directories(Utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})