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
     * ����mysql����
     */
    Connection();
    /**
     * �ر�mysql����
     */
    ~Connection();
    /**
     * ������mysql������
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
     * ����sql
     * @param sql
     * @return
     */
    bool update(std::string_view sql);
    /**
     * ��ѯsql
     * @param sql
     * @return
     */
    MYSQL_RES* query(std::string& sql);
private:
    MYSQL* _conn;
};


#endif //CPP_SERVER_TOOL_CONNECTION_H
