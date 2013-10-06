#ifndef Epoll_H
#include <iostream>
#include <sys/epoll.h>
#include <map>
#include <string>
#include <stdio.h>

template <class tClass>
struct eventItem
{
    struct epoll_event event;
    uint32_t timeout_ms;
    tClass * object;
    void ((tClass::*method)(uint32_t, uint16_t)); 
};

enum EVENT_FLAG {EV_ACCEPT, EV_READ};

using namespace std;
template <class T> class Epoll
{
    private:
        map < uint32_t, map < uint16_t ,struct eventItem<T> > > allEvents;
        uint32_t eventBase;
    public:
        Epoll();
        ~Epoll();
        int add(uint32_t fd, uint16_t flag, T *pObject, void ((T::*pMethod)(uint32_t, uint16_t)), uint32_t timeout_ms);
        void del(uint32_t fd, uint16_t flag);
        void loop();
};

#define Epoll_H
#endif
