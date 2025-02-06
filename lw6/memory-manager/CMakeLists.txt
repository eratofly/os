cmake_minimum_required(VERSION 3.22)
project(os)
enable_testing()

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "bin")

add_subdirectory(src)

add_executable(os main.cpp)

target_link_libraries(os PRIVATE ossrc)

add_subdirectory(tests)

add_dependencies(os run_tests)