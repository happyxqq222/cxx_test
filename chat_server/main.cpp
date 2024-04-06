
#include "ChatServer.h"
#include "mysql_conn_pool/DBConnectionPool.h"
#include "mysql_conn_pool/DBConnection.h"
#include "include/Singleton.h"
#include "include/ArrayBlockQueue.h"

#include <vector>
#include <iostream>
using namespace std;

mutex m;
vector<thread> v;


class Logic_System : public Singleton<Logic_System>
{
    friend class Singleton<Logic_System>;
public:
    ~Logic_System(){};
private:
    Logic_System() = default;
};

int main()
{
    ArrayBlockQueue<int> blockQueue(10);
    std::thread producer([&](){
        for (int i = 0; i < 5; ++i) {
            blockQueue.send(i);
            std::cout << "Sent: " << i << std::endl;
        }
        this_thread::sleep_for(5s);
        blockQueue.close();
    });
    std::thread consumer([&](){
        std::this_thread::sleep_for(500ms); // 故意延迟消费者开始消费
        int val;
        while(blockQueue.receive(val)){
            std::cout << "Received: " << val << std::endl;
        }
    });
    producer.join();
    consumer.join();

    return 0;
}