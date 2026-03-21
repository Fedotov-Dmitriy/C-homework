cmake_minimum_required(VERSION 3.25)
project(С-homework C)

add_library(avl_tree avl_tree.c)
target_include_directories(avl_tree PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(avl_tree_tests avl_tree_tests.c avl_tree_debug.c)

target_link_libraries(avl_tree_tests PRIVATE avl_tree)
set_target_properties(avl_tree avl_tree_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory("src/avl_tree")
add_subdirectory("src/flight_registration")
