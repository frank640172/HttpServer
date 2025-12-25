#pragma once
#include "Dispatcher.h"
#include <stdbool.h>
#include <pthread.h>

//全局变量
extern struct Dispatcher EpollDispatcher;
extern struct Dispatcher PollDispatcher;
extern struct Dispatcher SelectDispatcher;

enum ElemType {ADD, DELETE, MODIFY};

struct ChannelElement {
    //增加/修改/删除
    int type;
    struct Channel* channel;
    struct ChannelElement* next;
};
struct EventLoop {
    //分配器里面含有自己适配的 DispatcherData
    struct Dispatcher* dispatcher;

    void* dispatcherData;

    bool isRun; //是否运行

    //任务队列（存放 channel*的）
    struct ChannelElement* head;
    struct ChannelElement* tail;  

    struct ChannelMap* channelMap;

    //线程相关的信息
    pthread_t threadId;
    char threadName[32];

    //互斥锁
    //主线程加任务到队列，dispatcher 消费
    pthread_mutex_t mutex;

    //哨兵文件描述符，用于唤醒
    int socketPair[2];
};

//初始化主线程
struct EventLoop* eventLoopInit();

//初始化子线程（主线程名字是写死的）
struct EventLoop* eventLoopInitEx(const char* threadName);

//启动反应堆模型
int eventLoopRun(struct EventLoop* evLoop);

//处理激活的文件fd(3种模式用统一的 api函数去处理)
int eventActivate(struct EventLoop* evLoop, int fd, int event);

//添加任务到任务队列
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type);

//处理任务队列中的任务
int eventLoopProcessTask(struct EventLoop* evLoop);

int eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel);

int eventLoopRemove(struct EventLoop* evLoop, struct Channel* channel);

int eventLoopModify(struct EventLoop* evLoop, struct Channel* channel);