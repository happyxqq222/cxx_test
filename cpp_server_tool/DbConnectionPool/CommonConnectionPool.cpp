//
// Created by Administrator on 2024-02-25.
//
#include <fstream>
#include "pcb.h"
#include "CommonConnectionPool.h"

//�̰߳�ȫ���������к����ӿ�
ConnectionPool *ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;  //�б��������ɵļ����ͽ���
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
        if(idx == -1){   //��Ч��������
            continue;
        }
    }*/

    std::ifstream file("mysql.ini");
    std::string line;
    while(std::getline(file,line)){
        int idx = line.find('=',0);
        if(idx == -1){  //��Ч������
            continue;
        }
    }

    return false;
}
