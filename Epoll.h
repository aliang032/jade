#ifndef Epoll_H

#include <iostream>
#include <sys/epoll.h>
#include <map>
#include <string>
#include <stdio.h>

typedef void (*event_callback_fn)(uint32_t, uint16_t);

struct eventItem
{
    struct epoll_event event;
    event_callback_fn fn; 
};

enum EVENT_FLAG {EV_READ};

using namespace std;
class Epoll
{
    private:
        map < uint32_t, map < uint16_t ,struct eventItem > > allEvents;
        uint32_t eventBase;
    public:
        Epoll();
        ~Epoll();
        int add(uint32_t fd, uint16_t flag, event_callback_fn fn, void *args, uint32_t timeout);
        void loop();
};

#define Epoll_H
#endif
