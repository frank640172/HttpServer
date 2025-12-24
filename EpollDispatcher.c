#include "Dispatcher.h"
#include <sys/epoll.h>
#define MAX 512
struct EpollData {
    int epfd;
    struct epoll_event *events;
};

struct Dispatcher epollDispatcher = {
    epollInit,
    epollAdd, 
    epollRemove,
    epollModify,
    epollDispatcher,
    epollClear 
};

static void* epollInit();

static int epollAdd(struct Channel* channel, struct EventLoop* evLoop);

static int epollRemove(struct Channel* channel, struct EventLoop* evLoop);

static int epollModify(struct Channel* channel, struct EventLoop* evLoop);

static int epollDispatcher(struct EventLoop* evLoop, int timeout);
//清除数据
static int epollClear(struct EventLoop* evLoop);


static void* epollInit() {
    //epfd epoll_event
    struct EpollData* data = (struct EpollData*) malloc(sizeof(struct EpollData));
    data->epfd = epoll_create(1);
    if (data->epfd == -1) {
        perror("epoll create");
        return -1;
    }
    //calloc会申请空间并清 0
    data->events = (struct epoll_event *)colloc(MAX, sizeof(struct epoll_event));
    return data;
}

static int epollAdd(struct Channel* channel, struct EventLoop* evLoop){

}

static int epollRemove(struct Channel* channel, struct EventLoop* evLoop){

}

static int epollModify(struct Channel* channel, struct EventLoop* evLoop){

}

static int epollDispatcher(struct EventLoop* evLoop, int timeout){

}
//清除数据
static int epollClear(struct EventLoop* evLoop) {

}