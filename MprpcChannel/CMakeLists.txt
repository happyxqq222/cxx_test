cmake_minimum_required(VERSION 3.28)
project(mrpc)

set(CMAKE_CXX_STANDARD 17)

#生成debug版本，可以进行gdb调试
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
#设置项目可执行输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/output/lib)

find_package(Boost 1.85.0 REQUIRED)
find_package(protobuf REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(fmt REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

link_directories(${PROJECT_SOURCE_DIR}/output/lib)
link_libraries(protobuf::libprotobuf)
link_libraries(muduo_net muduo_base)
link_libraries(${Boost_LIBRARIES})
link_libraries(jsoncpp_lib)
link_libraries(fmt)

add_subdirectory(src)
add_subdirectory(example)