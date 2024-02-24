cmake_minimum_required(VERSION 3.27)
project(MemoryPool)

set(CMAKE_CXX_STANDARD 20)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)


add_executable(MemoryPool ${SRC})
