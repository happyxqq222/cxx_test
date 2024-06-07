cmake_minimum_required(VERSION 3.28)
project(client)

set(CMAKE_CXX_STANDARD 17)

find_package(jsoncpp REQUIRED)
find_package(TBB REQUIRED)

add_executable(${PROJECT_NAME}  main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE common)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)