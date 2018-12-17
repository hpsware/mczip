
add_executable(mczip_test
    Tester.cpp
    mczip_test.cpp
    testdata
)

target_link_libraries(mczip_test
    mczip)

file(COPY testdata DESTINATION ${CMAKE_BINARY_DIR}/test)