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

static int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int op);


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
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_ADD);
    if (ret == -1) {
        perror("epoll_ctl add");
        exit(0);
    }
    return ret;
}

static int epollRemove(struct Channel* channel, struct EventLoop* evLoop){
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_DEL);
    if (ret == -1) {
        perror("epoll_ctl del");
        exit(0);
    }
    return ret;
}

static int epollModify(struct Channel* channel, struct EventLoop* evLoop){
    int ret = epollCtl(channel, evLoop, EPOLL_CTL_MOD);
    if (ret == -1) {
        perror("epoll_ctl modify");
        exit(0);
    }
    return ret;
}


static int epollCtl(struct Channel* channel, struct EventLoop* evLoop, int op) {
    struct EpollData* data = (struct EpollData*)evLoop->dispatcherData;
    struct epoll_event event;
    event.data.fd = channel->fd;
    if (channel->events & ReadEvent) {
        event.events |= EPOLLIN;
    }
    if (channel->events & WriteEvent) {
        event.events |= EPOLLOUT;
    }

    int ret = epoll_ctl(data->epfd, op, channel->fd, &event);
    return ret;
}

static int epollDispatcher(struct EventLoop* evLoop, int timeout){
    struct EpollData* data = (struct EpollData*) evLoop->dispatcherData;
    int epfd = data->epfd;
    int num = epoll_wait(epfd, data->events, MAX,timeout * 1000);
    for (int i = 0; i < num; i++) {
        int events = data->events[i].events;
        int fd = data->events[i].data.fd;
        //如果是已经断开连接，或者断开连接还在发数据(异常)
        if (events & EPOLLERR || events & EPOLLHUP) {
            //删掉文件描述符, 通过channelMap
            //epollRemove(channel, evLoop);
            continue;
        }

        if (events & EPOLLIN) {
            evLoop->eventActivate(evLoop, fd, ReadEvent);
        }

        if (events & EPOLLOUT) {
            evLoop->eventActivate(evLoop, fd, WriteEvent);
        }

    }
    return 0;
}
//清除数据
static int epollClear(struct EventLoop* evLoop) {
    struct EpollData* data = (struct EpollData*)evLoop->dispatcherData;
    free(data->events);
    close(data->epfd);
    free(data);
}