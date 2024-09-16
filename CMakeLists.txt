cmake_minimum_required(VERSION 3.22)
project(os)

set(CMAKE_CXX_STANDARD 20)

include_directories(lw1)

add_executable(os
#        lw1/sys-info-win.cpp
        #lw1/sys-info-linux.cpp
        lw1/SysInfo.cpp
        lw1/SysInfo.h
        lw1/crossSysInfo.cpp)