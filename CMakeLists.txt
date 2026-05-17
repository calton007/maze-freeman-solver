cmake_minimum_required(VERSION 3.20)

project(maze_data_structure LANGUAGES CXX)

add_executable(maze maze.cpp)

target_compile_features(maze PRIVATE cxx_std_11)

if(MSVC)
    target_compile_options(maze PRIVATE /utf-8)
endif()
