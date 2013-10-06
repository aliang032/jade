#ifndef Worker_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include "Epoll.h"
#include "Epoll.cpp"

using namespace std;

class Worker
{
    private:
        int mainSock;
        Epoll<Worker> event;
    public:
        Worker(int sock);
        ~Worker();
        void serve();
        void ccbb(uint32_t fd, uint16_t flag);
};

typedef void (Worker::*workerMethod)(uint32_t , uint16_t);

#define Worker_H
#endif
