cmake_minimum_required(VERSION 3.28)
project(EncryptionTransfer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE srcs src/*.cpp src/*.cc)

message(STATUS "ss:${srcs}")


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(MUDUO_DIR "/usr/local/lib")

# add_compile_options(-lmuduo_base -lmuduo_net -lmuduo_http  -lpthread)

set(OpenSSL_ROOT_DIR "/usr/local/openssl")

find_package(Boost 1.85.0 REQUIRED COMPONENTS system thread container coroutine exception)
find_package(Protobuf REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(jsoncpp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(${PROJECT_NAME} ${srcs})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES} ${Protobuf_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE muduo_net muduo_base muduo_http pthread)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb TBB::tbbmalloc)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp)

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/CMakeLists.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>/a.txt)

# add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND echo "bin:$<TARGET_FILE_DIR:${PROJECT_NAME}>")
