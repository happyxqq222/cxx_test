cmake_minimum_required(VERSION 3.28)
project(Client)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE srcs src/*.cpp src/*.cc)

message(STATUS "ss:${srcs}")

set(PROJECT_NAME Client)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(MUDUO_DIR "/usr/local/lib")

# add_compile_options(-lmuduo_base -lmuduo_net -lmuduo_http  -lpthread)

find_package(Boost 1.85.0 REQUIRED COMPONENTS system thread container coroutine exception)
find_package(Protobuf REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${PROJECT_NAME} ${srcs})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE muduo_net muduo_base muduo_http pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb TBB::tbbmalloc)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})

