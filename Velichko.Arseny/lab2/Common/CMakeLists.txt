project(Common)

file(GLOB SOURCE_FILES *.cpp *.h)

add_library(Common ${SOURCE_FILES})
target_link_libraries(Common Utility ConnInterface)
target_include_directories(Common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})