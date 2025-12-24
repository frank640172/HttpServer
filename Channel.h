#pragma once
#include <stdbool.h>

typedef int (*handleFunc)(void* arg);

enum FDEvent {
    TimeOut = 0x1,
    ReadEvent = 0x2,
    WriteEvent = 0x4
}

struct Channel {
    int fd;
    int events;
    //回调函数
    handleFunc readCallback;
    handleFunc writeCallback; 
    //参数
    void* arg;
};

//对 channel的操作函数

//初始化一个 channel
struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg);

//修改 fd的写事件(写事件被检测或者不被检测)
void  writeEventEnable(struct Channel* channel, bool flag);

//判断是否需要检测当前文件描述符的写事件
bool  isWriteEventEnable(struct Channel* channel);