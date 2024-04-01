//
// Created by Administrator on 2024-03-31.
//

#ifndef MY_MUDUO_CALLBACKS_H
#define MY_MUDUO_CALLBACKS_H

#include "Timestamp.h"

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnection&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>;

using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer* T, Timestamp)>;


#endif //MY_MUDUO_CALLBACKS_H
