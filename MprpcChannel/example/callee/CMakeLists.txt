cmake_minimum_required(VERSION 3.28)
project(provider)


file(GLOB_RECURSE SRC_LIST ./*.cpp ./*.h ./*.cc)
message(${SRC_LIST})

add_executable(${PROJECT_NAME} ${SRC_LIST} ../user.pb.cc)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND cp ${PROJECT_SOURCE_DIR}/config.json ${EXECUTABLE_OUTPUT_PATH})