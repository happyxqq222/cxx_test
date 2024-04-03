
aux_source_directory(. mysql_srcs)

add_library(mysql_conn_pool SHARED ${mysql_srcs})