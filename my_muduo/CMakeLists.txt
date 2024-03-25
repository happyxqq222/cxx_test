cmake_minimum_required(VERSION 3.27)
project(my_muduo)

set(CMAKE_CXX_STANDARD 20)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)

message("${SRCS}")


if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif ()
find_package(boost_type_erasure REQUIRED)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
target_include_directories(${PROJECT_NAME} PRIVATE "/usr/local/include/")
