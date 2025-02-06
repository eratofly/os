cmake_minimum_required(VERSION 3.22.1)
project(student_db_lib)
add_library(StudentDB SHARED
        StudentDBCApi.cpp
        StudentDBCApi.h
        StudentDB.cpp
        StudentDB.h
        StudentDBWrapper.cpp
        StudentDBWrapper.h
)
target_include_directories(StudentDB PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Указываем флаг для C-совместимости
target_compile_definitions(StudentDB PUBLIC STUDENT_DATABASE_C_API)