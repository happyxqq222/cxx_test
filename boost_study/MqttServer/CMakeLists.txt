cmake_minimum_required(VERSION 3.28)
project(MqttServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
endif ()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)
find_package(Boost 1.85.0 REQUIRED COMPONENTS stacktrace_addr2line)
find_package(yaml-cpp REQUIRED)

add_subdirectory(third-party/spdlog)
add_subdirectory(third-party/jsoncpp)
add_subdirectory(third-party/cpp_properties)
include(FetchContent)


file(GLOB_RECURSE srcs ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE hrds ${PROJECT_SOURCE_DIR}/src/*.h)

include_directories(third-party/jsoncpp/include)
include_directories(third-party/spdlog/include)
include_directories(third-party/cpp_properties/src/include)
include_directories(src)
set(OPENSSL_ROOT_DIR "/usr/local/openssl")




add_executable(MqttServer main.cpp ${srcs})
target_link_libraries(${PROJECT_NAME} PUBLIC  spdlog::spdlog yaml-cpp::yaml-cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC tbb)

# if(MQ_WITH_TLS)
#     find_package(OpenSSL REQUIRED)
#     add_definitions(-DMQ_WITH_TLS)
#     target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::SSL)
# endif ()
add_definitions(-DMQ_WITH_TLS)
target_link_directories(${PROJECT_NAME} PUBLIC /usr/lib/x86_64-linux-gnu)
target_link_libraries(${PROJECT_NAME} PUBLIC ssl crypto)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

set(CERT_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output/bin)
add_custom_command(COMMAND 
            ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/certs ${CERT_OUTPUT_DIR}
            OUTPUT ${CERT_OUTPUT_DIR}/certificate.crt OUTPUT ${CERT_OUTPUT_DIR}/private_key.pem
            COMMENT "copy cert dir")

# add_custom_target(${PROJECT_NAME} ALL
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR} ${DESTINATION_DIR}
#                   COMMENT "Copying source_dir to destination_dir"
#                   VERBATIM)