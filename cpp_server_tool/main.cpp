#include <iostream>
#include "Connection.h"
#include "CommonConnectionPool.h"
#include <memory>
using namespace std;



int main() {

    ConnectionPool* connectionPool =  ConnectionPool::getConnectionPool();
    shared_ptr<Connection> conn  = connectionPool->getConnection();

    return 0;
}
