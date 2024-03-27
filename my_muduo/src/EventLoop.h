//
// Created by Administrator on 2024-03-24.
//

#ifndef MY_MUDUO_EVENTLOOP_H
#define MY_MUDUO_EVENTLOOP_H
#include <functional>
#include <vector>
#include <atomic>
#include <memory>

#include "Timestamp.h"
#include "noncopyable.h"
#include "CurrentThread.h"

class Channel;
class Poller;

class EventLoop :noncopyable{

public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    //����ʱ��ѭ��
    void loop();

    //�˳��¼�ѭ��
    void quit();

    Timestamp pollReturnTime() const{return pollReturnTime_;}

    //�ڵ�ǰloop��ִ�� cb
    void runInLoop(Functor cb);

    //��cb��������У�����loop���ڵ��߳� ִ��cb
    void queueInLoop(Functor cb);

    //��������loop���ڵ��̵߳�
    void wakeup();

    //EventLoop�ķ��� => Pooler�ķ���
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    //�ж�EventLoop�����Ƿ����Լ����߳���
    bool isInLoopThread() const{
        return threadId_ == CurrentThread::tid();
    }
private:
    void handleRead(Timestamp timestamp);
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_; //ԭ�Ӳ��� ͨ��CASʵ�ֵ�
    std::atomic_bool quit_;   //��ʶ �˳�loopѭ��

    const pid_t  threadId_;   //��¼��ǰloop�����̵߳�id

    Timestamp pollReturnTime_;   //����Poller���ط����¼���channels��ʱ���
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::atomic_bool callingPendingFunctors_; //��ʶ��ǰloop�Ƿ���Ҫִ�еĻص�����
    std::vector<Functor> pendingFunctors_;  //�洢loop��Ҫִ�е����лص�����
    std::mutex mutex_;  //������,������������vector�������̰߳�ȫ����
};


#endif //MY_MUDUO_EVENTLOOP_H
