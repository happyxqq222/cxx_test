cmake_minimum_required(VERSION 3.28)
project(MqttServer)

set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif ()
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output/lib)
find_package(Boost 1.85.0 REQUIRED)
find_package(yaml-cpp REQUIRED)
add_subdirectory(third-party/spdlog)
add_subdirectory(third-party/jsoncpp)
add_subdirectory(third-party/cpp_properties)
include(FetchContent)
add_compile_options(-Wno-error=unused-parameter)
#[[FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAGS master
)
if (NOT yaml-cpp_POPULATED)
    message(STATUS "Fetching yaml-cpp...")
    FetchContent_MakeAvailable(yaml-cpp)
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif ()]]

file(GLOB_RECURSE srcs ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE hrds ${PROJECT_SOURCE_DIR}/src/*.h)

include_directories(third-party/jsoncpp/include)
include_directories(third-party/spdlog/include)
include_directories(third-party/cpp_properties/src/include)
include_directories(src)
set(OPENSSL_ROOT_DIR "/usr/local/openssl")
add_executable(MqttServer main.cpp ${srcs})
target_link_libraries(${PROJECT_NAME} PUBLIC  spdlog::spdlog yaml-cpp::yaml-cpp)

# if(MQ_WITH_TLS)
#     find_package(OpenSSL REQUIRED)
#     add_definitions(-DMQ_WITH_TLS)
#     target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::SSL)
# endif ()
add_definitions(-DMQ_WITH_TLS)
target_link_directories(${PROJECT_NAME} PUBLIC /usr/lib/x86_64-linux-gnu)
target_link_libraries(${PROJECT_NAME} PUBLIC ssl crypto)
target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})
