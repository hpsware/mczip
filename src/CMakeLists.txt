cmake_minimum_required(VERSION 3.13)

set(source 
    zipper.cpp
    literal.cpp
    tagByte.cpp
    tuple.cpp
    mczip.cpp)

add_library(mczip
    ${source}
)

target_include_directories(
    mczip
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/detail
)
