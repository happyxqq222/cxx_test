cmake_minimum_required(VERSION 3.28)
project(common)

set(CMAKE_CXX_STANDARD  17)

add_library(common SHARED ByteBuffer.cc include/common/ByteBuffer.h include/common/Singleton.h include/common/BoundedBlockingQueue.h)

target_link_libraries(${PROJECT_NAME} PRIVATE muduo_net muduo_base muduo_http pthread)
target_include_directories(${PROJECT_NAME} PUBLIC include)