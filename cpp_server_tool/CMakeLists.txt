cmake_minimum_required(VERSION 3.27)
set(PROJECT_NAME cpp_server_tool)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(MYSQL_CONNECTOR_DIR C:/Program\ Files/MySQL/MySQL\ Server\ 5.7)
include_directories(DbConnectionPool json_parser tool)
#include_directories(${MYSQL_CONNECTOR_DIR}/include)
include_directories(${MYSQL_CONNECTOR_DIR}\\include)

aux_source_directory(${PROJECT_SOURCE_DIR}  SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/DbConnectionPool dbConnectionSRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/json_parser JsonParserSRC)

link_directories(${MYSQL_CONNECTOR_DIR}\\lib)
link_libraries(libmysql)
add_executable(${PROJECT_NAME} ${SRC} ${dbConnectionSRC} ${JsonParserSRC})
#target_link_libraries(${PROJECT_NAME} libmysql)
target_link_libraries(${PROJECT_NAME} libmysql)