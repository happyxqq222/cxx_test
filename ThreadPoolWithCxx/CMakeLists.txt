cmake_minimum_required(VERSION 3.26)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)
message(STATUS ${SRC})
add_executable(ThreadPool ${SRC})