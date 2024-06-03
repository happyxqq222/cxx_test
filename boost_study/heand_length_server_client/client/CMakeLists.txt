cmake_minimum_required(VERSION 3.28)
project(client)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.85.0 REQUIRED)
find_package(TBB REQUIRED)

add_executable(client main.cpp ByteBuffer.cc
        ../server/BoundedBlockingQueue.h)

target_link_libraries(client PRIVATE ${Boost_LIBRARIES})
target_link_libraries(client PRIVATE muduo_net muduo_base muduo_http pthread)
target_link_libraries(client PRIVATE tbb)