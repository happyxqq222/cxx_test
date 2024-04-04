
aux_source_directory(. mysql_srcs)

add_library(mysql_conn_pool SHARED ${mysql_srcs})
target_link_libraries(mysql_conn_pool PUBLIC mysqlclient)
target_link_directories(mysql_conn_pool PUBLIC .)