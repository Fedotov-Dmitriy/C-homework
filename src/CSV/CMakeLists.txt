add_executable(main main.c)

add_library(CSV CSV.c)

target_link_libraries(main PRIVATE CSV)

set(ASSETS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(ASSETS_BUILD_DIR "$<TARGET_FILE_DIR:main>/tests")

add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_SOURCE_DIR}
            ${ASSETS_BUILD_DIR}
    COMMENT "Copying assets directory to build output"