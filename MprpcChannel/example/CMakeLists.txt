cmake_minimum_required(VERSION 3.28)
project(example)

link_libraries(mprpc)

set(PROTO_FILE ${PROJECT_SOURCE_DIR}/user.proto)
set(GNEERATED_PROTO_SRCS ${PROJECT_SOURCE_DIR}/user.pb.cc)
set(GENERATED_RPOTO_HEADERS ${PROJECT_SOURCE_DIR}/user.pb.h)

message("bin:${CMAKE_BINARY_DIR}")
message("executable :${Protobuf_PROTOC_EXECUTABLE}")

add_custom_command(
    OUTPUT ${GNEERATED_PROTO_SRCS} ${GENERATED_RPOTO_HEADERS} PRE_BUILD
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --cpp_out = ${PROJECT_SOURCE_DIR} ${GENERATED_RPOTO_HEADERS}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating c++ code from ${PROTO_FILE}"
    VERBATIM
)

# add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${Protobuf_PROTOC_EXECUTABLE} --cpp_out =./ user.proto )



add_subdirectory(callee)
add_subdirectory(caller)