//
// Created by Administrator on 2024/6/3.
//

#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H
#include <functional>

#include "LogicNode.h"
#include "Session.h"
#include "common/BoundedBlockingQueue.h"
#include "common/Singleton.h"

using FunCallBack = std::function<void(std::shared_ptr<Session>, short msgId, const std::string& msgData)>;
using FunCallbackMap = tbb::concurrent_hash_map<short,FunCallBack>;
using LoginNodeQueue = xqq::BoundedBlockingQueue<std::shared_ptr<LogicNode>>;

enum MSG_TYPE {
    MSG_HELLO_WORLD = 1,
};

class LogicSystem  : public Singleton<LogicSystem>{
    friend class Singleton<LogicSystem>;

public:
    bool postMsgToQueue(const std::shared_ptr<LogicNode>& logiNode);
    void helloWorldCallback(std::shared_ptr<Session> session,  short msgId, const std::string& msgData);
    ~LogicSystem() override;
private:
    LogicSystem();
    void dealMsg();
    void registerCallbacks();
    std::thread workerThread_;
    LoginNodeQueue msgQueue_;
    FunCallbackMap funCallbacks_;
    bool stopFlag_;
};



#endif //LOGICSYSTEM_H
