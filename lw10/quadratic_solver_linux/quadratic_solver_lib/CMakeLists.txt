cmake_minimum_required(VERSION 3.22.1)
project(quadratic_solver_lib)

add_library(QuadraticSolverLib SHARED QuadraticSolverLib.cpp)

target_include_directories(QuadraticSolverLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
