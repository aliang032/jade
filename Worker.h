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
#include <errno.h>

using namespace std;

struct connectionItem
{
    uint32_t fd; 
    string recv_buf;
};

class Worker
{
    private:
        map<uint32_t, connectionItem> connections;
        uint32_t mainSock;
        Epoll<Worker> event;
        uint32_t currentConnection;
        uint32_t channel;
    public:
        static const uint32_t MAX_BUF_SIZE;
        Worker(int sock);
        ~Worker();
        void serve();
        void acceptConnection(uint32_t fd, uint16_t flag);
        void baseDealInput(uint32_t fd, uint16_t flag);
        int32_t dealInput(string &strData);
        void dealProcess(string &strData);
        int32_t sendToClient(const string & strData);
        int32_t sendToClient(string* strData);
        void closeConnection(uint32_t fd);
        void setChannel(uint32_t uiChannel);
        void dealCmd(uint32_t fd, uint16_t flag);
        void setNonblock(uint32_t fd);
};


#define Worker_H
#endif
