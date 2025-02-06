cmake_minimum_required(VERSION 3.22.1)
project(example)
add_executable(example example.cpp)
target_link_libraries(example PUBLIC QuadraticSolverLib)