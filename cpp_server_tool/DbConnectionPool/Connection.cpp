//
// Created by Administrator on 2024-02-25.
//

#include "Connection.h"
#include <iostream>
using namespace std::literals;
Connection::Connection() {
    //初始化数据库连接
    _conn = mysql_init(nullptr);
}

Connection::~Connection() {
    if(_conn != nullptr){
        mysql_close(_conn);
    }
}

bool
Connection::connect(const std::string &ip,
                    uint16_t port,
                    const std::string &user,
                    const std::string &password,
                    const std::string &dbname) {
    MYSQL *p = mysql_real_connect(_conn,
                                  ip.c_str(),
                                  user.c_str(),
                                  password.c_str(),
                                  dbname.c_str(),port,
                                  nullptr,
                                  0);
    return p != nullptr;
}




bool Connection::update(const std::string_view sql) {
    if(mysql_query(_conn, sql.data())){
        LOG("更新失败:"s + sql.data());
        return false;
    }
    return true;
}



MYSQL_RES *Connection::query(std::string& sql) {
    if(mysql_query(_conn,sql.c_str())){
        LOG("查询失败:"+sql);
        return nullptr;
    }
    return mysql_use_result(_conn);
}
