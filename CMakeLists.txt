cmake_minimum_required(VERSION 3.25)

project("json_lib") 

add_subdirectory(lib/data_structures)

option(DEBUG, OFF)

if (DEBUG)
  set(CMAKE_CXX_FLAGS "-O0 -g3 -lm")
endif()
set(CMAKE_CXX_FLAGS "-O3 -lm")

add_library(json_lib 
src/lexer.c 
src/token_vector_def.c 
src/parser.c 
src/char_vector_def.c
src/stringifier.c
src/utils.c
src/value_map_def.c
src/value_vector_def.c
src/destructor.c
)

target_link_libraries(json_lib PUBLIC data_structures)
target_include_directories(json_lib PUBLIC data_structures)

target_link_libraries(json_lib PUBLIC m)

if (DEBUG)
  add_executable(test_lexer test/test_lexer.c)
  target_link_libraries(test_lexer PUBLIC json_lib)
  add_executable(test_all test/test_all.c)
  target_link_libraries(test_all PUBLIC json_lib)
endif()