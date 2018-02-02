cmake_minimum_required(VERSION 3.2)

project(msgpassing)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

include_directories(libmsgpass)
add_library(msgpass libmsgpass/MessageQueue.cpp)

add_executable(helloworld helloworld.cpp)
target_link_libraries (helloworld msgpass pthread)

include_directories(test/catch)
add_executable(testmsgqueue test/msgqueue.cpp)
target_link_libraries (testmsgqueue msgpass pthread)
