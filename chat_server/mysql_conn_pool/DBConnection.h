//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_DBCONNECTION_H
#define CHAT_SERVER_DBCONNECTION_H

#include <mysql/mysql.h>
#include <string_view>
#include <string>
#include <optional>
#include <chrono>

#include "ResultSet.h"

#define MAX_ESCPE_STRING_LEN 10240


class DBConnectionPool;

class DBConnection {

public:
    DBConnection(DBConnectionPool*);

    DBConnection(DBConnection&& DBConnection);

    bool connect();


    bool executeCreate(std::string_view sqlQuery);

    bool executeDrop(std::string_view sqlQuery);

    std::optional<ResultSet> executeQuery(std::string_view sqlQuery);

    /**
     * 执行DB更新,修改
     * @param sqlQuery
     * @param careAffectedRows
     * @return
     */
    bool executeUpdate(std::string_view sqlQuery, bool careAffectedRows = true);

    uint32_t getInsertId();

    //开启事务
    bool startTransaction();

    //提交事务
    bool commit();

    //回滚事务
    bool rollback();

    //获取连接池
    std::string_view getPoolName();

    MYSQL *getMysql() { return mMysql; }

    void refreshAliveTime(){
        mAliveTime = std::chrono::steady_clock::now();
    };

    virtual ~DBConnection();

    //返回存活时间
    uint64_t getAliveTime() const{
       std::chrono::seconds d =  std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-mAliveTime);
       return d.count();
    }

    friend DBConnectionPool;
private:
    DBConnectionPool* mDBpool;
    MYSQL *mMysql;
    std::chrono::steady_clock::time_point mAliveTime;
};


#endif //CHAT_SERVER_DBCONNECTION_H
