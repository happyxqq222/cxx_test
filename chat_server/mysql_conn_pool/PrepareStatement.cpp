//
// Created by Administrator on 2024-04-04.
//

#include "PrepareStatement.h"

#include <strings.h>

PrepareStatement::PrepareStatement() : mStmt(nullptr), mParamBind(nullptr), mParamCnt(0){

}

PrepareStatement::~PrepareStatement() {
    if(mStmt){
        mysql_stmt_close(mStmt);
        mStmt = nullptr;
    }
    if(mParamBind){
        delete [] mParamBind;
        mParamBind = nullptr;
    }
}

bool PrepareStatement::init(MYSQL *mysql, std::string_view sql) {
    mysql_ping(mysql);   //当mysql连接丢失时候，使用mysql_ping能够自动重连数据库

    mStmt = mysql_stmt_init(mysql);
    if(!mStmt){
        printf("mysql_stmt_init failed\n");
        return false;
    }
    if(mysql_stmt_prepare(mStmt,sql.data(),sql.length())){
        printf("mysql_stmt_prepare failed: %s\n", mysql_stmt_error(mStmt));
        return false;
    }
    mParamCnt = mysql_stmt_param_count(mStmt);
    if(mParamCnt > 0 ){
        mParamBind = new MYSQL_BIND[mParamCnt];
        if(!mParamBind){
            printf("new MYSQL_BIND error\n");
            return false;
        }
        bzero(mParamBind, sizeof(MYSQL_BIND) * mParamCnt);
    }
    return true;
}

void PrepareStatement::setParam(uint32_t index, int &value) {
    if(index >= mParamCnt){
        printf("index too large: %d\n",index);
        return;
    }
    mParamBind[index].buffer_type = MYSQL_TYPE_LONG;
    mParamBind[index].buffer = &value;
}

void PrepareStatement::setParam(uint32_t index, uint32_t &value) {
    if(index >= mParamCnt){
        printf("index too large: %d\n",index);
        return;
    }
    mParamBind[index].buffer_type = MYSQL_TYPE_LONG;
    mParamBind[index].buffer = &value;
}

void PrepareStatement::setParam(uint32_t index, std::string &value) {
    if(index >= mParamCnt){
        printf("index too large: %d\n",index);
        return;
    }
    mParamBind[index].buffer_type = MYSQL_TYPE_STRING;
    mParamBind[index].buffer = const_cast<char*>(value.c_str());
    mParamBind[index].buffer_length = value.size();
}

void PrepareStatement::setParam(uint32_t index, const std::string &value) {
    if(index >= mParamCnt){
        printf("index too large: %d\n",index);
        return;
    }
    mParamBind[index].buffer_type = MYSQL_TYPE_STRING;
    mParamBind[index].buffer = const_cast<char*>(value.c_str());
    mParamBind[index].buffer_length = value.size();
}

bool PrepareStatement::executeUpdate() {
    if(!mStmt){
        printf("no stmt\n");
        return false;
    }
    if(mysql_stmt_bind_param(mStmt,mParamBind)){
        printf("mysql_stmt_bind_param failed: %s\n", mysql_stmt_error(mStmt));
        return false;
    }
    if(mysql_stmt_execute(mStmt)){
        printf("mysql_stmt_execute failed: %s\n", mysql_stmt_error(mStmt));
        return false;
    }
    if (mysql_stmt_affected_rows(mStmt) == 0)
    {
        printf("ExecuteUpdate have no effect\n");
        return false;
    }
    return true;
}

uint32_t PrepareStatement::getInsertId() {
    return mysql_stmt_insert_id(mStmt);
}
