cmake_minimum_required(VERSION 3.29)
project(test1)

set(CMAKE_CXX_STANDARD 17)

set(OpenSSL_DIR "/usr/local/openssl/")
find_package(OpenSSL REQUIRED )
find_package(jsoncpp REQUIRED)
find_package(Boost 1.85.0 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})