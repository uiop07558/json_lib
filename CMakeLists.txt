cmake_minimum_required(VERSION 3.25)

project("json_lib") 

add_subdirectory(lib/data_structures)

add_library(json_lib src/lexer.c)

target_link_libraries(json_lib PUBLIC lib/data_structures)

target_include_directories(json_lib PUBLIC lib/data_structures)