cmake_minimum_required(VERSION 3.28)
project(server)

set(CMAKE_CXX_STANDARD  17)

find_package(Boost 1.85.0 REQUIRED COMPONENTS thread)
find_package(TBB REQUIRED)
find_package(jsoncpp REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        Server.cpp
        Server.h
        Session.cpp
        Session.h
        SendNode.cpp
        SendNode.h
        LogicSystem.cpp
        LogicSystem.h
        LogicNode.cpp
        LogicNode.h
        AsioIOServicePool.cpp
        AsioIOServicePool.h
        AsioThreadPool.cpp
        AsioThreadPool.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
