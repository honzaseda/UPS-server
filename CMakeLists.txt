cmake_minimum_required(VERSION 3.3)
project(UPS_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp server.cpp server.h stl.cpp stl.h msgtable.cpp msgtable.h)
add_executable(UPS_server ${SOURCE_FILES})