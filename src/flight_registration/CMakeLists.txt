add_executable(main main.c)

target_link_libraries(main PRIVATE avl_tree)

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")