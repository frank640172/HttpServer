#pragma once
#include "Channel.h"
#include <stdbool.h>

struct ChannelMap {
    struct Channel **list;
    int sz;
};

//初始化
struct ChannelMap* channelMapInit(int sz);

//清空
void channelMapClear(struct ChannelMap* channelMap);

//扩容      元素个数   每个元素的大小（32 位指针大小是 4）
bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize);