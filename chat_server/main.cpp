#include <iostream>
#include "ChatServer.h"
#include "mysql_conn_pool/DBPool.h"
#include "mysql_conn_pool/DBConn.h"

using namespace std;

int main()
{
    DBPool dbPool("mypool","127.0.0.1",3306,"root","root","chat",16);
    for(int i =0; i < 1000;i++){
        thread t([&,i]{
            shared_ptr<DBConn> conn = dbPool.getDbConn(0);
            optional<ResultSet> resultSet = conn->executeQuery("select * from user");
            if (resultSet) {
                cout << "---------------------------------" << to_string(i) << "----------start" << endl;
                while (resultSet->next()) {
                    cout << resultSet->getString("name") << "," << resultSet->getString("password") << endl;
                }
                cout << "---------------------------------" << to_string(i) << "----------end" << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(10));
        });
        t.detach();
    }
    this_thread::sleep_for(2s);
    cout << dbPool.getDbConnectionCount() << endl;
    this_thread::sleep_for(15s);
    cout << dbPool.getDbConnectionCount() << endl;

    return 0;
}