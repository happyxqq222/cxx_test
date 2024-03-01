#include <iostream>
#include "Connection.h"
using namespace std;



int main1() {
    Connection conn;
    bool isConn = conn.connect("127.0.0.1",3306,"root","root","world");
    if(isConn){
//        std::cout << "connect success" << std::endl;
        LOG("connect success");
    }
    std::string updateSql = "insert into stu(username,password) values('abc','def')";
    bool isUpdate = conn.update(updateSql);
    if(isUpdate){
//        std::cout << "update success" << std::endl;
        LOG("update success");
    }

    return 0;
}
