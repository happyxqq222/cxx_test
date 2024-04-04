//
// Created by Administrator on 2024-04-04.
//

#ifndef CHAT_SERVER_RESULTSET_H
#define CHAT_SERVER_RESULTSET_H

#include <mysql/mysql.h>
#include <string_view>
#include <map>
#include <string>
#include <optional>


//返回结果 select时用
class ResultSet{
public:
    ResultSet(MYSQL_RES* res);
    virtual ~ResultSet();
    bool next();
    int getInt(const std::string& key);
    std::string getString(const std::string& key);
    ResultSet(const ResultSet& other) = default;
    ResultSet& operator=(const ResultSet& other) = default;
    ResultSet(ResultSet&& other);
    ResultSet& operator=(ResultSet&& other);
private:
    int getIndex(const std::string& key);

    MYSQL_RES* mRes;
    MYSQL_ROW mRow;
    std::map<std::string,int> mKeyMap;
};


#endif //CHAT_SERVER_RESULTSET_H
