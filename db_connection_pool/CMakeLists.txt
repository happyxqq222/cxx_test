cmake_minimum_required(VERSION 3.27)
project(db_connection_pool)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)

add_executable(db_connection_pool ${SRC})
