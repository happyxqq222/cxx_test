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
                 uint32_t port,
                 const std::string& user,
                 const std::string& password,
                 const std::string& dbname);
    /**
     * update sql
     * @param sql
     * @return
     */
    bool update(std::string_view sql);
    /**
     * query sql
     * @param sql
     * @return
     */
    MYSQL_RES* query(std::string& sql);

    void refreshAliveTime(){
        _alivetime = clock();
    };
    clock_t  getAliveTime() {
        return clock() - _alivetime;
    };
private:
    MYSQL* _conn;
    clock_t _alivetime; //idle time
};


#endif //CPP_SERVER_TOOL_CONNECTION_H
