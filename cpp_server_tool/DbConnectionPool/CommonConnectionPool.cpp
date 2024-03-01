//
// Created by Administrator on 2024-02-25.
//
#include <fstream>
#include "pcb.h"
#include "CommonConnectionPool.h"

//线程安全的懒汉单列函数接口
ConnectionPool *ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;  //有编译器生成的加锁和解锁
    return &pool;
}

ConnectionPool::ConnectionPool() {

}

bool ConnectionPool::loadConfigFile() {
/*    FILE *pf = fopen("mysql.ini","r");
    if(pf == nullptr){
        LOG("mysql.ini file is not exist!");
        return false;
    }

    char line[1024] = {0};
    while(!feof(pf)){
        memset(line,0,1024);
        fgets(line,1024,pf);
        std::string str = line;
        int idx = str.find('=',0);
        if(idx == -1){   //无效的配置项
            continue;
        }
    }*/

    std::ifstream file("mysql.ini");
    std::string line;
    while(std::getline(file,line)){
        int idx = line.find('=',0);
        if(idx == -1){  //无效的配置
            continue;
        }
    }

    return false;
}
