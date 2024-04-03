//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_CDBCONN_H
#define CHAT_SERVER_CDBCONN_H

#include <mysql/mysql.h>
#include <string_view>
#include <string>

#include "CResultSet.h"

#define MAX_ESCPE_STRING_LEN 10240

class CDBPool;


class CDBConn {

public:
    CDBConn(CDBPool *pDBPool);

    virtual ~CDBConn();

    int init();

    bool executeCreate(std::string_view sqlQuery);

    bool executeDrop(std::string_view sqlQuery);

    CResultSet *executeQuery(std::string_view sqlQuery);

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
    std::string getPoolName();

    MYSQL *getMysql() { return mMsql; }

private:
    CDBPool *mPDBpool;  //  to get mysql server information
    MYSQL *mMsql;
    char mEscapeString[MAX_ESCPE_STRING_LEN + 1];
};


#endif //CHAT_SERVER_CDBCONN_H
