cmake_minimum_required(VERSION 3.16)
project(cpp)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(cpp
        Sudoku.cpp
        main.cpp
        Sudoku.hpp)