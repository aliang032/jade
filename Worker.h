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
        map<uint32_t, string> recvBuf;
        uint32_t mainSock;
        Epoll<Worker> event;
    public:
        static const uint32_t MAX_BUF_SIZE;
        Worker(int sock);
        ~Worker();
        void serve();
        void acceptConnection(uint32_t fd, uint16_t flag);
        void readFromConnection(uint32_t fd, uint16_t flag);
};

typedef void (Worker::*workerMethod)(uint32_t , uint16_t);

#define Worker_H
#endif
