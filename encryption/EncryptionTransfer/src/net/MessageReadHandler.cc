
#include "EncryptionTransfer/net/MessageReadHandler.h"
#include <any>

std::any MessageReadHandler::handleRead(std::any& args) {
    return args;
}

void MessageReadHandler::handle(std::any& args) {
    std::any retAny = handleRead(args);
    if(nextHandler != nullptr) {
        nextHandler->handle(retAny);
    }
}

void MessageReadHandler::addHandler(std::shared_ptr<MessageReadHandler>& handler) {
    nextHandler = handler;
}