#pragma once
#include "Channel.h"
#include "EventLoop.h"

struct Dispatcher {
    //init
    //初始化 epoll/poll/select需要用到的数据
    //返回的是 struct DispatcherData*
    void* (*init)();
    //add
    int (*add)(struct Channel* channel, struct EventLoop* evLoop);
    //del
    int (*remove)(struct Channel* channel, struct EventLoop* evLoop);

    //modify
    int (*modify)(struct Channel* channel, struct EventLoop* evLoop);

    //事件检测
    //还需要调用回调，需要 channelMap
    //超时事件
    int (*dispatcher)(struct EventLoop* evLoop, int timeout);

    //清除数据
    int (*clear)(struct EventLoop* evLoop);
};