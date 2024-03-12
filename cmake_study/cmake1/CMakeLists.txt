cmake_minimum_required(VERSION 3.27)
project(cmake1 VERSION 1.2)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
add_definitions(-finput-charset=UTF-8 -fexec-charset=GBK -Wall)

option(USER_MYMATH "use my math?" OFF)

if (USER_MYMATH)
    add_subdirectory(MathFunctions)
    list(APPEND EXTRA_LIBS  MathFunctions)
    list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif ()

message(STATUS ${USER_MYMATH})

message(STATUS "Hello World")


set(STR_TEST "Hello World")

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")



target_compile_options(virtual_library INTERFACE
  "$<${gcc_like_cxx}:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>" "$<${msvc_cxx}:-W3>")



add_executable(cmake1 main.cpp)
message(STATUS "MAJOR:${cmake1_VERSION_MAJOR}")
message(STATUS "MINOR:${cmake1_VERSION_MINOR}")
target_include_directories(cmake1 PUBLIC "${PROJECT_BINARY_DIR}")

configure_file(makeTest.h.in makeTest.h)
target_link_libraries(cmake1 PRIVATE ${EXTRA_LIBS} virtual_library)

target_compile_definitions(cmake1 PRIVATE EX3)

install(TARGETS cmake1 DESTINATION bin)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
message(STATUS ${CMAKE_AR})
#add_custom_command(TARGET
#        ${PROJECT_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
#        COMMENT "cleaning up the output directory before build"
#        )
