cmake_minimum_required(VERSION 3.20)

project(dfa_number LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(dfa dfa.c)
target_include_directories(dfa PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(dfa_number main.c)
target_link_libraries(dfa_number PRIVATE dfa)

add_executable(dfa_tests dfa_tests.c)
target_link_libraries(dfa_tests PRIVATE dfa)

set_target_properties(
  dfa
  dfa_number
  dfa_tests
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

enable_testing()
add_test(NAME dfa_tests COMMAND dfa_tests)

message(STATUS "Included ${CMAKE_CURRENT_SOURCE_DIR} directory for DFA library.")
