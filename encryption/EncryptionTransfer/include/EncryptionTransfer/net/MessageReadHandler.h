#pragma once
#include <any>
#include <memory>

class MessageReadHandler {
public:
    virtual std::any handleRead(std::any& args);
    void handle(std::any& args);
    virtual ~MessageReadHandler() = default;
    void addHandler(std::shared_ptr<MessageReadHandler>& handler);
private:
    std::shared_ptr<MessageReadHandler> nextHandler;
};




