cmake_minimum_required(VERSION 2.8)

project( enkiMI )

add_executable(enkimi_example)
target_sources(enkimi_example
        PRIVATE
        example.c)
target_link_libraries(enkimi_example
        PRIVATE
        enkiMI::enkimi)
#copy our example minecraft chunk file so our example runs regardless of directory the source exists in.
add_custom_command(
        TARGET enkimi_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/r.1.0.mca
                ${CMAKE_CURRENT_BINARY_DIR}/r.1.0.mca
        )