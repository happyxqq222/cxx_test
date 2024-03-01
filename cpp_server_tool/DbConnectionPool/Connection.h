//
// Created by Administrator on 2024-02-25.
//

#ifndef CPP_SERVER_TOOL_CONNECTION_H
#define CPP_SERVER_TOOL_CONNECTION_H
#include "public.h"
#include <string>
#include <mysql.h>
#include <cstdint>

class Connection {

public:
    /**
     * 创建mysql连接
     */
    Connection();
    /**
     * 关闭mysql连接
     */
    ~Connection();
    /**
     * 建立与mysql的连接
     * @param ip
     * @param port
     * @param user
     * @param password
     * @param dbname
     * @return
     */
    bool connect(const std::string& ip,
                 uint16_t port,
                 const std::string& user,
                 const std::string& password,
                 const std::string& dbname);
    /**
     * 更新sql
     * @param sql
     * @return
     */
    bool update(std::string_view sql);
    /**
     * 查询sql
     * @param sql
     * @return
     */
    MYSQL_RES* query(std::string& sql);
private:
    MYSQL* _conn;
};


#endif //CPP_SERVER_TOOL_CONNECTION_H
