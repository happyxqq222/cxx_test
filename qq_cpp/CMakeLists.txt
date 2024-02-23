cmake_minimum_required(VERSION 3.27)
project(qq_cpp)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)

add_executable(qq_cpp ${SRC})
