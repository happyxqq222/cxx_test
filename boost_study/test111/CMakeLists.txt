cmake_minimum_required(VERSION 3.28)
project(test111)

set(CMAKE_CXX_STANDARD 17)

add_executable(test111 main.cpp
        BoundedBlockingQueue.h)
