cmake_minimum_required(VERSION 3.28)
project(server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.85.0 REQUIRED)
find_package(TBB REQUIRED)


add_executable(${PROJECT_NAME} main.cpp
        const.h
        Server.cpp
        Server.h
        Session.cpp
        Session.h
        ByteBuffer.cc
        BoundedBlockingQueue.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE muduo_net muduo_base muduo_http pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE tbb)