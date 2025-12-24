#include "ChannelMap.h"
#include <stdio.h>
#include <stdlib.h>

struct ChannelMap* channelMapInit(int sz) {
  struct ChannelMap* channelMap = (struct ChannelMap*) malloc(sizeof(struct ChannelMap));
  channleMap->sz = sz;
  channelMap->list = (struct Channel**) malloc(sz * sizeof(struct Channel)); 
  return channelMap; 
}

void channelMapClear(struct ChannelMap* channelMap) {
   if (channelMap != NULL) {
        int sz = channelMap->sz;
        for(int i = 0; i < sz; i++) {
            if(channelMap->list[i] != NULL) {
                free(channelMap->list[i]);
            }
        }
        free(channelMap->list);
        channelMap->sz = 0;
   }
}

/*
bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize) {
    if (map->sz < newSize) {
        struct ChannelMap* channelMap = channelMapInit(newSize);
        int sz = map->sz;
    
        for (int i = 0; i < sz; i++) {
            channelMap->list[i] = map->list[i];
        }

        //释放掉老的，创建新的
        channelMapClear(map);
    }
    return false;
}
*/

bool makeMapRoom(struct ChannelMap* map, int newSize, int unitSize) {
    if (map->sz < newSize) {
        int curSize = map->sz;
        while(curSize < newSize) {    
            curSize = curSize * 2;
        }
       struct Channel **tmp = realloc(map->list, newSize * unitSize);
       if(tmp == NULL){
            return false;
       }
       map->list = tmp;
       memset(&map->list[map->sz], 0, (curSize - map->sz) * unitSize);
        map->sz = curSize;
    }
    return true;
}

