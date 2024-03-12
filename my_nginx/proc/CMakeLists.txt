file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)
add_library(proc SHARED ${srcs})

target_include_directories(proc PUBLIC ${PROJECT_SOURCE_DIR}/_include)