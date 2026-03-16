cmake_minimum_required(VERSION 3.25)
project(example C)

add_library(ExampleLib ExampleLib.c)

add_executable(main main.c)
target_link_libraries(main PRIVATE ExampleLib)
