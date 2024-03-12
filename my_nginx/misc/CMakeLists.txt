file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)
add_library(misc SHARED ${srcs})
target_include_directories(misc PUBLIC ${PROJECT_SOURCE_DIR}/_include)