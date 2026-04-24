add_executable(states states.c)

target_link_libraries(states PRIVATE graph)
target_link_libraries(states PRIVATE heap)

set_target_properties(states PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")