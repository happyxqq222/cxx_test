//
// Created by Administrator on 2024-02-25.
//

#ifndef CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#define CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "Connection.h"


class ConnectionPool {
public:
    static ConnectionPool* getConnectionPool();
private:
    ConnectionPool();  //����#���캯��˽�л�
    //�������ļ��м�������
    bool loadConfigFile();
private:
    std::string _ip;
    uint16_t _port;
    std::string _username;
    std::string _password;
    uint32_t _initSize;   //��ʼ������
    uint32_t _maxSize;    //���ӳ����������
    uint32_t _maxIdleTime;  //���ӳ�������ʱ��
    uint32_t _connectionTimeout;   //���ӳػ�ȡ���ӵĳ�ʱʱ��

    std::queue<Connection*> _connectionQueue; //�洢mysql���ӵĶ���
    std::recursive_mutex _queueMutex;   //ά�����Ӷ��е��̻߳��ⰲȫ��
};


#endif //CPP_SERVER_TOOL_COMMONCONNECTIONPOOL_H
