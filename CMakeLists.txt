CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(Toff)

MESSAGE(STATUS "Project: Toff C++ Server")
MESSAGE(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

SET(CMAKE_BUILE_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c11")

SET(SOURCE_FILES http_server.cpp http_server.h error_code.h)