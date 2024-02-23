cmake_minimum_required(VERSION 3.26)
project(CasDqueue)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)

add_executable(CasDqueue ${SRC})
