#pragma once
#include "Dispatcher.h"

//全局变量
extern struct Dispatcher epollDispatcher;


struct EventLoop {
    //分配器里面含有自己适配的 DispatcherData
    struct Dispatcher* dispatcher;

    void* dispatcherData;

};