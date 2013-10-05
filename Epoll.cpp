#include "Epoll.h"
using namespace std;

Epoll::Epoll()
{
    this->eventBase = epoll_create(100);
}

Epoll::~Epoll()
{
}

int Epoll::add(uint32_t fd, uint16_t flag, event_callback_fn fn, void * args, uint32_t timeout)
{
     switch(flag)
     {
        case EV_READ:
           map<uint16_t, struct eventItem> mapEventItems;
           struct eventItem stEventItem;
           mapEventItems.insert(pair<uint16_t, struct eventItem>(flag, stEventItem));
           this->allEvents.insert(pair<uint32_t, map<uint16_t, struct eventItem> >(fd, mapEventItems));
           this->allEvents[fd][flag].event.data.fd = fd;
           this->allEvents[fd][flag].event.events=EPOLLIN | EPOLLET;
           this->allEvents[fd][flag].fn = fn;
           epoll_ctl(this->eventBase, EPOLL_CTL_ADD, fd, &this->allEvents[fd][flag].event);
           break;
        
     }
     return 0;
}
         
void Epoll::loop()
{
    uint32_t i, uiCount, uiConnectFd;
    struct epoll_event stEvents[100];
    uiCount = epoll_wait(this->eventBase, stEvents, 100, -1);
    for(i = 0; i < uiCount; i++)
    {
        uint32_t fd = stEvents[i].data.fd;
        if(this->allEvents.find(fd) == this->allEvents.end())
        {
            printf("fd:%d not found in this->allEvents\n", fd);
            continue;
        }
        map<uint16_t, struct eventItem>::iterator it;
        for(it=this->allEvents[fd].begin(); it != this->allEvents[fd].end(); ++it)
        {
            cout<<"nana"<<it->first;
            switch(it->first)
            {
                case EV_READ:
                    (*(it->second.fn))(fd, EV_READ);
                    cout<<it->second.fn<<"\n";
                    break;

            }
        }
    }
}



