add_library(heap heap.c)
target_include_directories(heap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(heapTest heapTest.c)

target_link_libraries(heapTest PRIVATE heap)

set_target_properties(heapTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(heap ${SANITIZER_LIBS})
endif()