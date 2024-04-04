//
// Created by Administrator on 2024-04-04.
//

#include "ResultSet.h"


using namespace std;

ResultSet::ResultSet(MYSQL_RES *res) : mRes(res) {
    int fieldNum = mysql_num_fields(mRes);
    MYSQL_FIELD *fields = mysql_fetch_field(mRes);
    for (int i = 0; i < fieldNum; i++) {
        mKeyMap.insert({fields[i].name,i});
    }
}

ResultSet::ResultSet(ResultSet &&other)
        : mRes(other.mRes),
          mRow(other.mRow),
          mKeyMap(std::move(other.mKeyMap)) {
    other.mRes = nullptr;
    other.mRow = nullptr;
}

ResultSet &ResultSet::operator=(ResultSet &&other) {
    if(this == &other){
        return *this;
    }
    this->mRes = other.mRes;
    this->mRow = other.mRow;
    this->mKeyMap = std::move(other.mKeyMap);
    other.mRes = nullptr;
    other.mRow = nullptr;
    return *this;
}

ResultSet::~ResultSet() {
    if(mRes){
        mysql_free_result(mRes);
        mRes = nullptr;
    }
}

bool ResultSet::next() {
    mRow = mysql_fetch_row(mRes);
    if(mRow){
        return true;
    }
    return false;
}

int ResultSet::getIndex(const string& key) {
    auto it = mKeyMap.find(key);
    if(it == mKeyMap.end()){
        return -1;
    }else{
        return it->second;
    }
}

int ResultSet::getInt(const std::string& key){
    int idx = getIndex(key);
    if(idx == -1){
        return 0;
    }else{
        return std::atoi(mRow[idx]);
    }
}

string ResultSet::getString(const std::string& key) {
    int idx = getIndex(key);
    if(idx == -1){
        return string();
    }else{
        return mRow[idx];
    }
}


