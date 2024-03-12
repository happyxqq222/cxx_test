cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)



add_executable(test1 ${srcs})
target_link_libraries(test1 PUBLIC net)


add_test(
        NAME mytest1
        COMMAND test1
)