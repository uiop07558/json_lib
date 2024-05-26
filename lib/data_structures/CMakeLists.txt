cmake_minimum_required(VERSION 3.12)

project("data_structures")

set(CMAKE_CXX_FLAGS "-O3")

add_library(data_structures hashtable/hash.c)

target_link_libraries(data_structures PUBLIC m)