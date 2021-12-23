project(Tests)

file(GLOB SOURCE_FILES *.cpp *.h)

add_executable(run_tests ${SOURCE_FILES})
target_link_libraries(run_tests PRIVATE Structures Utility)
