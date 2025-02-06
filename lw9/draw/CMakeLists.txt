cmake_minimum_required(VERSION 3.22.1)

project(os)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(os draw.cpp)

target_link_libraries(os Qt5::Widgets)