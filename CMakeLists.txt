cmake_minimum_required(VERSION 2.8.9)

project(Renderer)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(renderer ${SOURCES})