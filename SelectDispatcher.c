#include "Dispatcher.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/type.h>
#include <unistd.h>
#include <stdio.h>
/*
*/
#define MAX 1024
struct SelectData {
    fd_set readSet;
    fd_set writeSet;

};

struct Dispatcher SelectDispatcher = {
    selectInit,
    selectAdd,
    selectRemove,
    selectModify,
    selectDispatcher,
    selectClear
};

static void* selectInit();

static int selectAdd(struct Channel* channel, struct EventLoop* evLoop);

static int selectRemove(struct Channel* channel, struct EventLoop* evLoop);

static int selectModify(struct Channel* channel, struct EventLoop* evLoop);

static int selectDispatcher(struct EventLoop* evLoop, int timeout);
//�������
static int selectClear(struct EventLoop* evLoop);

static void setFdSet(struct Channel* channel, struct SelectData* data);
static void clearFdSet(struct Channel* channel, struct SelectData* data);



static void* selectInit() {
    struct SelectData* data = (struct SelectData*)malloc(sizeof(struct SelectData));
    FD_ZERO(&data->readSet);
    FD_ZERO(&data->writeSet);
    return data;
}



static int selectAdd(struct Channel* channel, struct EventLoop* evLoop) {
    struct SelectData* data = (struct SelectData*)evLoop->dispatcherData;
    if (channel -> fd >= MAX) {
        return -1;
    }

    setFdSet(channel, data); 
    return 0;
}

static int selectRemove(struct Channel* channel, struct EventLoop* evLoop) {
    struct SelectData* data = (struct SelectData*)evLoop->dispatcherData;
     if (channel -> fd >= MAX) {
        return -1;
    }
   
    selectClear(channel, data);
    return 0;
}

static int selectModify(struct Channel* channel, struct EventLoop* evLoop) {
    struct SelectData* data = (struct SelectData*)evLoop->dispatcherData;

    //这里有 bug，留到后面弄，那个里面就会现在让你监听的函数
    //只是弄了事件，但我不知道具体是要添加还是删除
    return 0;
}

static int selectDispatcher(struct EventLoop* evLoop, int timeout) {
    struct SelectData* data = (struct SelectData*)evLoop->dispatcherData;
    struct timeval val;
    val.tv_sec = timeout;
    val.tv_usec = 0;
    fd_set read_bak = data->readSet;
    fd_set write_bak = data->writeSet; 
    //对原始数据进行备份，因为是传入传出参数
    int num = select(MAX, read_bak, write_bak, NULL, &val);
    if (num == -1) {
        perror("selectDispatcher");
        return -1; 
    }
    for (int i = 0; i < MAX; i++) {
       
        //������Ѿ��Ͽ����ӣ����߶Ͽ����ӻ��ڷ�����(�쳣)
     
        if (FD_ISSET(i, &read_bak)) {
              evLoop->eventActivate(evLoop, i, ReadEvent);
        }

        if (FD_ISSET(i, &write_bak)) {
            evLoop->eventActivate(evLoop, i, WriteEvent);
        }

    }
     
}   
//�������
static int selectClear(struct EventLoop* evLoop) {
    struct SelectData* data = (struct SelectData*)evLoop->dispatcherData;
    free(data);
}

static void setFdSet(struct Channel* channel, struct SelectData* data) {
    if (channel->events & ReadEvent) {
        FD_SET(channel->fd, &data->readSet);
    }
    if (channel->events & WriteEvent) {
        FD_SET(channel->fd, &data->writeSet);
    }
}

static void clearFdSet(struct Channel* channel, struct SelectData* data) {
    if (channel->events & ReadEvent) {
        FD_CLR(channel->fd, &data->readSet);
    }
    if (channel->events & WriteEvent) {
        FD_CLR(channel->fd, &data->writeSet);
    }
}