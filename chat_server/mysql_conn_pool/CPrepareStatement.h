//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_CPREPARESTATEMENT_H
#define CHAT_SERVER_CPREPARESTATEMENT_H

#include <mysql/mysql.h>
#include <string>
#include <string_view>

//插入数据用
class CPrepareStatement{
public:
    CPrepareStatement();
    virtual ~CPrepareStatement();
    bool init(MYSQL* mysql, std::string& sql);

    void setParam(uint32_t index, int& value);
    void setParam(uint32_t index, uint32_t& value);
    void setParam(uint32_t index, std::string& value);
    void setParam(uint32_t index, const std::string& value);

    bool executeUpdate();
    uint32_t getInsertId();

private:
    MYSQL_STMT* mStmt;
    MYSQL_BIND* mParamBind;
    uint32_t mParamCnt;
};


#endif //CHAT_SERVER_CPREPARESTATEMENT_H
