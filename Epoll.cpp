#include "Epoll.h"

using namespace std;

template <class T>
Epoll<T>::Epoll()
{
    this->eventBase = epoll_create(100);
}

template <class T>
Epoll<T>::~Epoll()
{
}

template <class T>
int Epoll<T>::add(uint32_t fd, uint16_t flag, T * pObject, void ((T::*pMethod)(uint32_t, uint16_t)), uint32_t timeout_ms)
{
     switch(flag)
     {
        case EV_ACCEPT:
        case EV_READ:
           map<uint16_t, struct eventItem<T> > mapEventItems;
           struct eventItem<T> stEventItem;
           mapEventItems.insert(pair<uint16_t, struct eventItem<T> >(flag, stEventItem));
           this->allEvents.insert(pair<uint32_t, map<uint16_t, struct eventItem<T> > >(fd, mapEventItems));
           this->allEvents[fd][flag].event.data.fd = fd;
           this->allEvents[fd][flag].event.events = EPOLLIN | EPOLLET;
           this->allEvents[fd][flag].timeout_ms = timeout_ms;
           this->allEvents[fd][flag].object = pObject;
           this->allEvents[fd][flag].method = pMethod;
           epoll_ctl(this->eventBase, EPOLL_CTL_ADD, fd, &this->allEvents[fd][flag].event);
           break;
        
     }
     return 0;
}

template <class T>
int Epoll<T>::del(uint32_t fd, uint16_t flag)
{
    if(this->allEvents.find(fd) != this->allEvents.end() && this->allEvents[fd].find(flag) != this->allEvents[fd].end())
    {
        
    }
}

template <class T>         
void Epoll<T>::loop()
{
    uint32_t i, uiCount, uiConnectFd;
    struct epoll_event stEvents[100];
    for(;;)
    {
    uiCount = epoll_wait(this->eventBase, stEvents, 100, 1000);
    if(uiCount == 0)
    {
        typename map < uint32_t, map < uint16_t ,struct eventItem<T> > >::iterator it;
        typename map<uint16_t, struct eventItem<T> >::iterator it2;
        for(it = this->allEvents.begin();it != this->allEvents.end(); ++it)
        {
            uint32_t fd = it->first;
            uint16_t flag;
            for(it2 = it->second.begin();it2 != it->second.end(); ++it2)
            {
                flag = it2->first;
                if(it2->second.timeout_ms != 0)
                {
                     //time out
                     if(this->allEvents.find(fd) != this->allEvents.end() && this->allEvents[fd].find(flag) != this->allEvents[fd].end())
                     {
                        (it2->second.object->*it2->second.method)(fd, flag);
                     }
                }
            }
        } 
        continue;
    }
    for(i = 0; i < uiCount; i++)
    {
        uint32_t fd = stEvents[i].data.fd;
        if(this->allEvents.find(fd) == this->allEvents.end())
        {
            printf("fd:%d not found in this->allEvents\n", fd);
            continue;
        }
        typename map<uint16_t, struct eventItem<T> >::iterator it;
        for(it=this->allEvents[fd].begin(); it != this->allEvents[fd].end(); ++it)
        {
            switch(it->first)
            {
                case EV_ACCEPT:
                case EV_READ:
                    (it->second.object->*it->second.method)(fd, EV_READ);
                    cout<<"done\n";
                    break;

            }
        }
    }
    }
}



