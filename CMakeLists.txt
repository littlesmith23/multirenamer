cmake_minimum_required(VERSION 3.30)
project(multirenamer)

set(CMAKE_CXX_STANDARD 20)

add_executable(multirenamer main.cpp
        multirenamer.cpp
        multirenamer.h)

target_include_directories(multirenamer PUBLIC ./include/)
