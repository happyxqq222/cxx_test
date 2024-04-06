cmake_minimum_required(VERSION 3.27)
project(chat_server)

set(CMAKE_CXX_STANDARD 17)

find_package(jsoncpp REQUIRED)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(mysql_conn_pool)
add_subdirectory(thread_pool)

#file(GLOB_RECURSE srcs *.cpp)
aux_source_directory(. srcs)
message(STATUS "-----------------")
message(STATUS ${srcs})
message(STATUS "-----------------")
add_executable(chat_server ${srcs})
target_link_libraries(chat_server PRIVATE muduo_net muduo_base pthread jsoncpp mysql_conn_pool thread_pool)
