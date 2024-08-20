cmake_minimum_required(VERSION 3.28)
project(src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS false)

aux_source_directory(. SRC_LIST)

add_library(mprpc STATIC ${SRC_LIST})
target_link_libraries(mprpc PRIVATE muduo_net muduo_base pthread)