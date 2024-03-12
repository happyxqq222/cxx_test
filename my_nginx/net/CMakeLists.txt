file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)
add_library(net SHARED ${srcs})
target_include_directories(net PUBLIC ${PROJECT_SOURCE_DIR}/_include)