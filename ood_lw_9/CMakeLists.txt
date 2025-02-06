cmake_minimum_required(VERSION 3.22.1)
project(os)

set(CMAKE_CXX_STANDARD 20)

add_executable(os main.cpp
        drawer.cpp
        cow.h
        Image.cpp
        tile.h
ImageService.h
ImageService.cpp)
