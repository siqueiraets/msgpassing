cmake_minimum_required(VERSION 3.2)

project(concurrency)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

#include_directories( src/core src/gui src/modules src/io)

# Main application
add_executable(concurrency main.cpp)
target_link_libraries (concurrency pthread)
