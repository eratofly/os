cmake_minimum_required(VERSION 3.22)
project(os)

set(CMAKE_CXX_STANDARD 20)

add_executable(os
        graphApp.cpp
)

find_package(wxWidgets REQUIRED)

if (wxWidgets_FOUND)
    include_directories(${wxWidgets_INCLUDE_DIRS})
    message(STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
    message(STATUS "wxWidgets_INCLUDE_DIRS: ${wxWidgets_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "wxWidgets library not found")
endif ()

target_link_libraries(os ${wxWidgets_LIBRARIES})
