project(Utility)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.h)
add_library(Utility ${SOURCE_FILES})
target_include_directories(Utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Utility librt.so libpthread.so)