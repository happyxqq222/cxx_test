//
// Created by Administrator on 2024-04-04.
//

#include "DBConn.h"
#include "DBPool.h"

using namespace std;

DBConn::DBConn(DBPool *pDBPool) : mDBpool(pDBPool), mMysql(nullptr){
}

DBConn::~DBConn() {
    if(mMysql){
        mysql_close(mMysql);
    }
}

bool DBConn::connect() {
    mMysql = mysql_init(nullptr);
    if(!mMysql){
        printf("mysql_init failed\n");
        return false;
    }
    int reconnect = 1;
    mysql_options(mMysql,MYSQL_OPT_RECONNECT, &reconnect);
    mysql_options(mMysql,MYSQL_SET_CHARSET_NAME, "utf8mb4");

    if(!mysql_real_connect(mMysql, mDBpool->getDbServerIp().data(),
                           mDBpool->getUsername().data(),mDBpool->getPassword().data(),
                           mDBpool->getDbName().data(),mDBpool->getDbServerPort(),
                           nullptr,0)){
        printf("mysql_real_connect failed: %s\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

bool DBConn::executeCreate(std::string_view sqlQuery) {
    mysql_ping(mMysql);
    if(mysql_real_query(mMysql,sqlQuery.data(),sqlQuery.length())){
        printf("mysql_real_query failed: %s, sql: start transaction\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

bool DBConn::executeDrop(std::string_view sqlQuery) {
    mysql_ping(mMysql);
    if(mysql_real_query(mMysql,sqlQuery.data(),sqlQuery.length())){
        printf("mysql_real_query failed: %s, sql: start transaction\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

optional<ResultSet> DBConn::executeQuery(std::string_view sqlQuery) {
    mysql_ping(mMysql);
    if(mysql_real_query(mMysql,sqlQuery.data(),sqlQuery.length())){
        printf("mysql_real_query failed: %s, sql: %s\n", mysql_error(mMysql), sqlQuery.data());
        return nullopt;
    }
    //返回结果
    MYSQL_RES* res = mysql_store_result(mMysql);
    if(!res){
        printf("mysql_store_result failed: %s\n", mysql_error(mMysql));
        return nullopt;
    }
    return ResultSet(res);
}

bool DBConn::executeUpdate(std::string_view sqlQuery, bool careAffectedRows) {
    mysql_ping(mMysql);

    if(mysql_real_query(mMysql,sqlQuery.data(),sqlQuery.length())){
        printf("mysql_real_query failed: %s, sql: %s\n", mysql_error(mMysql), sqlQuery.data());
        return false;
    }
    if(mysql_affected_rows(mMysql) > 0 ){
        return true;
    }else{
        if(careAffectedRows){
            // 如果在意影响的行数时, 返回false, 否则返回true
            printf("mysql_real_query failed: %s, sql: %s\n", mysql_error(mMysql), sqlQuery.data());
            return false;
        }else{
            printf("affected_rows=0, sql: %s\n", sqlQuery.data());
            return true;
        }
    }
}

uint32_t DBConn::getInsertId() {
    return (uint32_t) mysql_insert_id(mMysql);
}

bool DBConn::startTransaction() {
    mysql_ping(mMysql);
    if(mysql_real_query(mMysql,"start transaction\n",17)){
        printf("mysql_real_query failed: %s, sql: start transaction\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

bool DBConn::commit() {
    mysql_ping(mMysql);

    if(mysql_real_query(mMysql,"commit\n",6)){
        printf("mysql_real_query failed: %s, sql: commit\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

bool DBConn::rollback() {
    mysql_ping(mMysql);

    if(mysql_real_query(mMysql,"rollback\n",8)){
        printf("mysql_real_query failed: %s, sql: rollback\n", mysql_error(mMysql));
        return false;
    }
    return true;
}

std::string_view DBConn::getPoolName() {
    return mDBpool->getPoolName();
}
