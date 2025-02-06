cmake_minimum_required(VERSION 3.22.1)
project(quadratic_solver_lib)
#что означает SHARED
add_library(QuadraticSolverLib SHARED QuadraticSolverLib.cpp QuadraticSolverLib.h)

target_include_directories(QuadraticSolverLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
