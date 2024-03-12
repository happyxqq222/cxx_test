file(GLOB_RECURSE srcs CONFIGURE_DEPENDS ./*.cpp)
add_library(log SHARED ${srcs})

target_include_directories(log PUBLIC include)