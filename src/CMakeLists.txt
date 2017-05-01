cmake_minimum_required(VERSION 2.8)

project(Toff)

message(STATUS "Project: Toff C++ Server")
message(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

set(CMAKE_BUILE_TYPE DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES http_server.cpp http_server.h error_code.h request_handler.cpp request_handler.h)

include_directories("./")

add_executable(HelloWorld hello_world.cpp)