#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/TcpConnection.h>
#include <oneapi/tbb/concurrent_hash_map.h>
#include <iostream>
#include <cstdio>
#include <string>
#include "EncryptionTransfer/biz/ClientOp.h"
#include "EncryptionTransfer/net/Client.h"
#include "EncryptionTransfer/tool/RSAUtils.h"


using namespace std;
using namespace muduo;
using namespace muduo::net;


int main() {
    cout << "连接服务器..." << endl;
    RSAUtil rsaUtil;
    rsaUtil.generate();
    Client client(rsaUtil);
    client.start();
    cout << "连接服务器成功" << endl;
    ClientOp clientOp{rsaUtil,client};
    string menuStr = R"(
        /****************************************************************/
        /****************************************************************/
        /*    1.密钥协商                                                 */
        /*    2.密钥校验                                                 */
        /*    3.密钥注销                                                 */
        /*    4.密钥查看                                                 */
        /*    0.退出系统                                                 */
        /****************************************************************/
        /****************************************************************/
    )";
    for(;;) {
        cout << menuStr << endl;
        int sel = -1;
        sel = getchar();
        switch (sel) {
            case '1':
            clientOp.seckeyArgree();
            break;
            case '2':
            break;
            case '3':
            break;
            case '4':
            break;
        }
        getchar();
    }



    return 0;
}