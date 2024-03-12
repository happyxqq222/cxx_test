file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)

add_executable(app ${srcs})

target_include_directories(app PUBLIC ${PROJECT_SOURCE_DIR}/_include include)
target_link_libraries(app PUBLIC misc net proc signal)
#target_compile_options(app PRIVATE -fsanitize=address -g)

#target_link_libraries(app PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)

add_custom_command(TARGET app COMMAND
        ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/nginx.conf" "$<TARGET_FILE_DIR:app>/")

add_custom_command(TARGET app COMMAND echo "${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/nginx.conf $<TARGET_FILE_DIR:app>/")