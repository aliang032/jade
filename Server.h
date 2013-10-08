#ifndef Server_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h> 
#include "Epoll.h"
#include <errno.h>
#include "Worker.h"

using namespace std;

enum ProtocolName{TCP, UDP};

struct ConfigItem
{
    string ip;
    uint16_t port;
    uint16_t child_count;
    ProtocolName protocol;
    uint32_t recv_timeout;
    uint32_t process_timeout;
    uint32_t send_timeout;
};

class Server 
{
    private:
        map<string, ConfigItem> config;
        map<string, uint32_t> listenedSockets;
        map<string, map<pid_t, pid_t> > workerPids;
        map<pid_t, int32_t> channels;
        Epoll<Server> event;
    public:
        static const uint32_t MAX_BUF_SIZE;
        Server();
        ~Server();
        void run();
        void createSocketsAndListen();
        void createWorkers();
        void forkOneWorker(const string &strWorkerName);
        void dealCmdResult(uint32_t fd, uint16_t flag);
        void setNonblock(uint32_t fd);
        void loop();
};


#define Server_H
#endif
