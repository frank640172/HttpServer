#include "Dispatcher.h"
#include <poll.h>
/*
*/
#define MAX 1024
struct PollData {
    int maxfd;//���������λ��
    struct pollfd fds[MAX]; //����������
};

struct Dispatcher PollDispatcher = {
    pollInit,
    pollAdd,
    pollRemove,
    pollModify,
    pollDispatcher,
    pollClear
};

static void* pollInit();

static int pollAdd(struct Channel* channel, struct EventLoop* evLoop);

static int pollRemove(struct Channel* channel, struct EventLoop* evLoop);

static int pollModify(struct Channel* channel, struct EventLoop* evLoop);

static int pollDispatcher(struct EventLoop* evLoop, int timeout);
//�������
static int pollClear(struct EventLoop* evLoop);



static void* pollInit() {
    struct PollData* data = (struct PollData*)malloc(sizeof(struct PollData));
    data->maxfd = 0;
    for (int i = 0; i < MAX; i++) {
        data->fds[i].fd = -1;
        data->fds[i].events = data->fds[i].revents = 0;
    }
    return data;
}

static int pollAdd(struct Channel* channel, struct EventLoop* evLoop) {
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int events = 0;
    if (channel->events & ReadEvent) {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent) {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < MAX; i++) {
        if (data->fds[i].fd == -1) {
            data->fds[i].fd = channel->fd;
            data->fds[i].events = events;
            data->maxfd = i > data->maxfd ? i : data->maxfd;
            break;
        }
    }

    if (i >= MAX) {
        return -1;
    }
    return 0;
}

static int pollRemove(struct Channel* channel, struct EventLoop* evLoop) {
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
   
    int i = 0;
    for (; i < MAX; i++) {
        if (data->fds[i].fd == channel->fd) {
            data->fds[i].fd = -1;
            data->fds[i].events = 0;
            data->fds[i].revents = 0;
            break;
        }
    }

    if (i >= MAX) {
        return -1;
    }
    return 0;
}

static int pollModify(struct Channel* channel, struct EventLoop* evLoop) {
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int events = 0;
    if (channel->events & ReadEvent) {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent) {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < MAX; i++) {
        if (data->fds[i].fd == channel->fd) {
            data->fds[i].events = events;
            break;
        }
    }

    if (i >= MAX) {
        return -1;
    }
    return 0;
}

static int pollDispatcher(struct EventLoop* evLoop, int timeout) {
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int num = poll(data->fds, data->maxfd + 1, timeout * 1000);
    if (num == -1) {
        perror("pollDispatcher");
        return -1; 
    }
    for (int i = 0; i < num; i++) {
       
        if (data->fds[i].fd == -1) {
            continue;
        }
        //������Ѿ��Ͽ����ӣ����߶Ͽ����ӻ��ڷ�����(�쳣)
     
        if (data->fds[i].revents & POLLIN) {
             evLoop->eventActivate(evLoop, data->fds[i].fd, ReadEvent);
        }

        if (data->fds[i].revents & POLLOUT) {
             evLoop->eventActivate(evLoop, data->fds[i].fd, WriteEvent);
        }

    }
     
}   
//�������
static int pollClear(struct EventLoop* evLoop) {
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    free(data);
}