

aux_source_directory(. threadpoolsrcs)

add_library(thread_pool SHARED ${threadpoolsrcs})

target_link_libraries(thread_pool PUBLIC pthread)

target_link_directories(thread_pool PUBLIC .)