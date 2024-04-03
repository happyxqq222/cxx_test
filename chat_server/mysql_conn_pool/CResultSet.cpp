//
// Created by Administrator on 2024-04-04.
//

#include "CResultSet.h"


using namespace std;

CResultSet::CResultSet(MYSQL_RES *res) : mRes(res) {
    int fieldNum = mysql_num_fields(mRes);
    MYSQL_FIELD *fields = mysql_fetch_field(mRes);
    for (int i = 0; i < fieldNum; i++) {
        mKeyMap.insert({fields[i].name,i});
    }
}

CResultSet::~CResultSet() {
    if(mRes){
        mysql_free_result(mRes);
        mRes = nullptr;
    }
}

bool CResultSet::next() {
    mRow = mysql_fetch_row(mRes);
    if(mRow){
        return true;
    }
    return false;
}

int CResultSet::getIndex(const string& key) {
    auto it = mKeyMap.find(key);
    if(it == mKeyMap.end()){
        return -1;
    }else{
        return it->second;
    }
}

int CResultSet::getInt(const std::string& key){
    int idx = getIndex(key);
    if(idx == -1){
        return 0;
    }else{
        return std::atoi(mRow[idx]);
    }
}

string CResultSet::getString(const std::string& key) {
    int idx = getIndex(key);
    if(idx == -1){
        return string();
    }else{
        return mRow[idx];
    }
}


