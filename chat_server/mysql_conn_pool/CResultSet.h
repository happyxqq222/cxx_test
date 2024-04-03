//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_CRESULTSET_H
#define CHAT_SERVER_CRESULTSET_H

#include <mysql/mysql.h>
#include <string_view>
#include <map>
#include <string>
#include <optional>


//返回结果 select时用
class CResultSet{
public:
    CResultSet(MYSQL_RES* res);
    virtual ~CResultSet();
    bool next();
    int getInt(const std::string& key);
    std::string getString(const std::string& key);
private:
    int getIndex(const std::string& key);

    MYSQL_RES* mRes;
    MYSQL_ROW mRow;
    std::map<std::string,int> mKeyMap;
};


#endif //CHAT_SERVER_CRESULTSET_H
