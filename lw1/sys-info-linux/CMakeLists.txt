cmake_minimum_required(VERSION 3.22)
project(os)

set(CMAKE_CXX_STANDARD 20)

include_directories(lw1/sys-info-linux)

add_executable(os
        lw1/sys-info-linux.cpp
)