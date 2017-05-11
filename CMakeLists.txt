cmake_minimum_required(VERSION 2.8)

project(Toff)

message(STATUS "Project: Toff C++ Server")
message(STATUS "Project Source Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "Project Binary Directory: ${PROJECT_BINARY_DIR}")

set(CMAKE_BUILE_TYPE DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")


add_subdirectory("./src")
add_subdirectory("./libs")
