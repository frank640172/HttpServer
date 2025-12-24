#include "Channel.h"


struct Channel* channelInit(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
    struct Channel* channel = (struct Channel*)malloc(sizeof(struct Channel));
    channel->fd = fd;
    channel->events = events;
    channel->readFunc = readFunc;
    channel->writeFunc = writeFunc;
    channel->args = args; 
    return channel;
}

void  writeEventEnable(struct Channel* channel, bool flag) {
   if (flag == true)
        channel-> events |= WriteEvent;
    else
        channel-> events =  channel-> events & ~WriteEvent;
}

bool  isWriteEventEnable(struct Channel* channel) {
    return channel->events & WriteEvent; 
}