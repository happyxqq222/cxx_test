cmake_minimum_required(VERSION 3.28)
project(coroutine)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.85.0 REQUIRED COMPONENTS thread)
find_package(TBB REQUIRED)
find_package(jsoncpp REQUIRED)

add_executable(${PROJECT_NAME} main.cpp
        main2.cpp)


target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
