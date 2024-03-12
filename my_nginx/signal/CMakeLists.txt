file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)
message("srcs:${srcs}")
add_library(signal SHARED ${srcs})
target_include_directories(signal PUBLIC ${PROJECT_SOURCE_DIR}/_include include)