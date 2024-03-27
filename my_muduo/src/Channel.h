//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_CHANNEL_H
#define MY_MUDUO_CHANNEL_H

#include <functional>
#include <memory>

#include "noncopyable.h"

class EventLoop;

class Timestamp;


class Channel : noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);

    ~Channel();

    void handleEvent(Timestamp receiveTime);

    //设置回调函数
    void setReadCallback(ReadEventCallback cb){
        readCallback_ = std::move(cb);
        update();
    }
    void setWriteCallback(const EventCallback& cb){
        writeCallback_ = std::move(cb);
        update();
    }
    void setCloseCallback(const EventCallback& cb){
        closeCallback_ = std::move(cb);
        update();
    }
    void setErrorCallback(const EventCallback& cb){
        errorCallback_ = std::move(cb);
        update();
    }

    //防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const{
        return fd_;
    }
    int events()const {
        return events_;
    }
    void set_revents(int revt){
        revents_ = revt;
    }


    //设置fd相应的事件状态
    void enableReading(){
        events_ |= kReadEvent;
    }
    void disableReading(){
        events_ &= ~kReadEvent;
    }
    void enableWriting(){
        events_ |= kWriteEvent;
    }
    void disableWriting(){
        events_ &= ~kWriteEvent;
    }
    void disableAll(){
        events_ = kNoneEvent;
    }

    //返回fd当前事件状态
    bool isNoneEvent() const {
        return events_ == kNoneEvent;
    }
    bool isWriting() const{
        return events_ & kWriteEvent;
    }
    bool isReading() const{
        return events_ & kReadEvent;
    }
    int index(){
        return index_;
    }
    void set_index(int index){
        index_ = index;
    }

    EventLoop* ownerLoop(){
        return loop_;
    }
    void remove();
private:

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;  //event loop
    const int fd_;  // fd, Pooler监听的对象
    int events_;   //注册fd感兴趣的事件
    int revents_;  //pooler返回的具体发生的事件
    int index_;
    std::weak_ptr<void> tie_;
    bool tied_;
    ReadEventCallback  readCallback_;
    EventCallback  writeCallback_;
    EventCallback  closeCallback_;
    EventCallback errorCallback_;
};


#endif //MY_MUDUO_CHANNEL_H
