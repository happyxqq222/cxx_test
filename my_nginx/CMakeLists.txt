cmake_minimum_required(VERSION 3.25)
project(mynginx)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
add_definitions("-DUNICODE" "-D_UNICODE")

#add_definitions("-fsanitize=address" "-g" "-O1")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_LIST_DIR}/output/lib/)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/output/bin/)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_LIST_DIR}/output/bin/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/output/bin/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/output/bin/)

#configure_file(nginx.conf ${PROJECT_SOURCE_DIR}/bin COPYONLY)   #copy file to destination directory

message("cmake_install_prefix :${CMAKE_INSTALL_PREFIX}")



enable_testing()

add_subdirectory(app)
add_subdirectory(net)
add_subdirectory(misc)
add_subdirectory(signal)
add_subdirectory(proc)
add_subdirectory(log)
add_subdirectory(test)








