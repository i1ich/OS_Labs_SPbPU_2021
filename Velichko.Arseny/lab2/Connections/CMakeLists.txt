project(Connections)

add_library(ConnInterface INTERFACE)
target_include_directories(ConnInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB CONN_SOURCES *.cpp *.h)
file(GLOB CONN_IMPLS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/Core Core/conn_*.cpp)

set(CONNECTIONS "")

foreach(CONN_IMPL ${CONN_IMPLS})
    string(REGEX MATCH "(conn_)([A-Za-z0-9_]+)(.cpp)" CONN_IMPL "${CONN_IMPL}")
    set(CONN_NAME "${CMAKE_MATCH_2}")
    list(APPEND CONNECTIONS "${CONN_NAME}")

    add_library(conn_${CONN_NAME} Core/conn_${CONN_NAME}.h Core/conn_${CONN_NAME}.cpp ${CONN_SOURCES})
    target_compile_definitions(conn_${CONN_NAME} PRIVATE CONN_IMPL_HEADER="Core/conn_${CONN_NAME}.h")
    target_link_libraries(conn_${CONN_NAME} ConnInterface Utility)
endforeach()

message(STATUS "${CONNECTIONS}")
set(CONNECTIONS "${CONNECTIONS}" CACHE INTERNAL "")