//
// Created by Administrator on 2024-04-04.
//

#include "CPrepareStatement.h"

CPrepareStatement::CPrepareStatement() :mStmt(nullptr),mParamBind(nullptr),mParamCnt(0){

}

CPrepareStatement::~CPrepareStatement() {
    if(mStmt){
        mysql_stmt_close(mStmt);
        mStmt = nullptr;
    }
    if(mParamBind){
        delete [] mParamBind;
        mParamBind = nullptr;
    }
}

bool CPrepareStatement::init(MYSQL *mysql, std::string &sql) {
    mysql_ping(mysql);   //当mysql连接丢失时候，使用mysql_ping能够自动重连数据库
    mStmt = mysql_stmt_init(mysql);
    if(!mStmt){

    }
    return false;
}

void CPrepareStatement::setParam(uint32_t index, int &value) {

}

void CPrepareStatement::setParam(uint32_t index, uint32_t &value) {

}

void CPrepareStatement::setParam(uint32_t index, std::string &value) {

}

void CPrepareStatement::setParam(uint32_t index, const std::string &value) {

}

bool CPrepareStatement::executeUpdate() {
    return false;
}

uint32_t CPrepareStatement::getInsertId() {
    return 0;
}
