#include "EventLoop.h"
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>
 #include <sys/types.h>

struct EventLoop* eventLoopInit(){
    return eventLoopInitEx(NULL);
}

void taskWakeup(struct EventLoop* evLoop) {
    const char* msg = "hello world";
    write(evLoop->socketPair[0], msg, strlen(msg));
}

//接收传递的参数
int readLocalMessage(void* arg) {
   struct EventLoop* evLoop = (struct EventLoop*) arg;
   char buf[256];
   read(evLoop->socketPair[1], buf, sizeof(buf));
   return 0;
}
 
struct EventLoop* eventLoopInitEx(const char* threadName) {
    struct EventLoop* evLoop = (struct EventLoop*) malloc(sizoef (struct EventLoop));
    //没运行，这个是是否运行
    evLoop->isRun = false;
    evLoop->threadID = pthread_self();
    pthread_mutex_init(&evLoop->mutex, NULL);
    strcpy(evLoop->threadName, threadName == NULL ? "MainThread": threadName);
    evLoop->dispatcher = &EpollDispatcher;
    evLoop->dispatcherData = evLoop->dispatcher->init();
    evLoop->head = evLoop->tail = NULL;
    evLoop->channelMap->channelMapInit(128);
    //ipv4/ipv6 本地套接字通信
    int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, evLoop->socketPair);
    if (ret == -1) {
       perror("socketpair");
       exit(0);
    }

    //socketPair[0]发数据，socketPair[1]接收数据，把 socketPair封装放到队列中
    struct Channel* channel = channelInit(evLoop->socketPair[1], ReadEvent, 
                readLocalEvent, NULL, evLoop);
    eventLoopAddTask(evLoop, channel, ADD);
    return evLoop;
}

int eventLoopRun(struct EventLoop* evLoop) {
        assert(evLoop != NULL);

        if (evLoop->threadID != pthread_self()) {
            return -1;
        }
        struct Dispatcher* dispatcher = evLoop->dispatcher;
        //循环进行事件处理
        while (evLoop->isRun) {
            //不仅要处理监听的描述对应的回调函数，还要处理任务队列里的函数
            dispatcher->dispatcher(evLoop, 2); //超时时长
            eventLoopProcessTask(evLoop);
        }
        return 0;
}

int eventActivate(struct EventLoop* evLoop, int fd, int event) {
   if (evLoop == NULL || fd < 0) {
        return -1;
   }
   //取出对应的channel,之后 channel根据读/写来处理
   struct Channel* channel= evLoop->channelMap->list[fd];
   assert(channel->fd == fd);

   if (event & ReadEvent && channel->readCallback) {
        channel->readCallback(channel->arg);
   }
   if (event & WriteEvent && channel->writeCallback) {
        channel->writeCallback(channel->arg);
   }
   return 0; 
}

//添加任务到任务队列
int eventLoopAddTask(struct EventLoop* evLoop, struct Channel* channel, int type) {
    //加锁，保护共享资源
    pthread_mutex_lock(&evLoop->mutex);
    struct ChannelElement* ele = (struct ChannelElement*)malloc(sizeof (struct ChannelElement)); 
    ele->channel = channel;
    ele->type = type;
    ele->next = NULL;
    //链表为空
    if(evLoop->head == NULL) {
        evLoop->head = evLoop->tail = ele;
    } else {
        //头插
        /*
        ele->next = evLoop->head->next;
        evLoop->head->next = ele;
        */
        //尾插
        evLoop->tail->next = ele;
        evLoop->tail = ele;
    }
    pthread_mutex_unlock(&evLoop->mutex);
    //处理节点（根据 type）
    //添加节点可能是主线程，通信是子线程
    //这里暂时不考虑单反应堆模型（就是没子线程，只有一个主线程的情况）
    //如果有子线程，主线程原则上不会给自己添加通信的东西，只负责连接
    if (pthread_self() == evLoop->threadID) {
            eventLoopProcessTask(evLoop);
    } else {
        //告诉子线程你可以去处理任务了
        //子线程正在工作/子线程阻塞
        taskWakeup(evLoop);
    }

    return 0;
}

int eventLoopProcessTask(struct EventLoop* evLoop) {
    pthread_mutex_lock(evLoop->mutex);
    struct ChannelElement* head = evLoop->head;
    while(head != NULL) {
        struct Channel* channel = head->channel;
        if (head->type == ADD) {

        } else if (head->type == DELETE) {

        } else if (head->tyep == MODIFY) {

        }
        struct ChannelElement* head = tmp;
        head = head -> next;
        free(tmp);
        
    }
    evLoop->head = evLoop->tail = NULL;
    pthread_mutex_unlock(evLoop->mutex);
    return 0;
}

int eventLoopAdd(struct EventLoop* evLoop, struct Channel* channel) {
    int fd = channel -> fd;
    struct ChannelMap*  channelMap = evLoop->channelMap;
    if (fd >= channelMap->sz) {
        // 扩容
        if(!makeMapRoom(channelMap, fd, sizeof(struct Channel*))){
            return -1;
        }
    }
    if (channelMap->list[fd] == NULL) {
       channelMap->list[fd] = channel; 
       evLoop->dispatcher->add(channel, evLoop);
    }
    return 0;
}

int eventLoopRemove(struct EventLoop* evLoop, struct Channel* channel) {

}

int eventLoopModify(struct EventLoop* evLoop, struct Channel* channel) {

}
