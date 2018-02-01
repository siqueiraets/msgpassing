cmake_minimum_required(VERSION 3.2)

project(concurrency)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

include_directories(libmsgpass)
add_library(libmsgpass libmsgpass/MessageQueue.cpp)

# Main application
add_executable(concurrency main.cpp)
target_link_libraries (concurrency libmsgpass pthread)

include_directories(test/catch)
add_executable(testmsgqueue test/msgqueue.cpp)
target_link_libraries (testmsgqueue libmsgpass pthread)
