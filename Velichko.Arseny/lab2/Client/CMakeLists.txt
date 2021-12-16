project(Client)

file(GLOB SOURCE_FILES *.cpp *.h)

foreach(CONN_NAME ${CONNECTIONS})
    add_executable(client_${CONN_NAME} ${SOURCE_FILES})
    target_link_libraries(client_${CONN_NAME} PUBLIC Common conn_${CONN_NAME} Utility)
endforeach()