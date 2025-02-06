cmake_minimum_required(VERSION 3.22.1)
project(example)
add_executable(example example.cpp)
target_link_libraries(example PRIVATE QuadraticSolverLib)

add_custom_command(TARGET example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:QuadraticSolverLib>
        $<TARGET_FILE_DIR:example>
)