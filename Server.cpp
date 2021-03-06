#include "Server.h"

using namespace std;


    Server::Server()
    {
        ConfigItem stConfigItem;
        this->config.insert(pair<string, ConfigItem>("worker1", stConfigItem));
        this->config["worker1"].ip = "0.0.0.0";
        this->config["worker1"].port = 4401;
        this->config["worker1"].protocol = TCP;
        this->config["worker1"].child_count = 4;
        this->config.insert(pair<string, ConfigItem>("worker2", stConfigItem));
        this->config["worker2"].ip = "0.0.0.0";
        this->config["worker2"].port = 4402;
        this->config["worker2"].protocol = TCP;
        this->config["worker2"].child_count = 1;
    }
 
    Server::~Server()
    {

    }
    
    void Server::run()
    {
        this->createSocketsAndListen();
        this->createWorkers();
        this->loop(); 
    }

    void Server::createSocketsAndListen()
    {
        int32_t sockFd;
        struct sockaddr_in stSockAddr;
        typename map<string, ConfigItem>::iterator it;
        for(it = this->config.begin(); it != this->config.end(); it++)
        { 
            int32_t iType = it->second.protocol == TCP ? SOCK_STREAM : SOCK_DGRAM;
            if((sockFd = socket(AF_INET, iType, 0)) == -1)
            {
               printf("socket create err\n");
               exit(110);
            }
            stSockAddr.sin_family = AF_INET;
            stSockAddr.sin_port = htons(it->second.port);
            stSockAddr.sin_addr.s_addr = inet_addr(it->second.ip.c_str());
            memset(&(stSockAddr.sin_zero), 0, sizeof(stSockAddr.sin_zero));
            if(bind(sockFd, (struct sockaddr *)&stSockAddr, sizeof(struct sockaddr)) == -1)
            {
                printf("bind fail\n");
                exit(111);
            }
            if(listen(sockFd, 10) == -1)
            {
                printf("listen fail\n");
                exit(112);
            }
            this->listenedSockets.insert(pair<string, uint32_t>(it->first, sockFd));

        }
    }

    void Server::createWorkers()
    {
       typename map<string, uint32_t>::iterator it;
       for(it = this->listenedSockets.begin(); it != this->listenedSockets.end(); it++)
       {
           string strWorkerName = it->first;
           while(this->workerPids.find(strWorkerName) == this->workerPids.end() || this->config[strWorkerName].child_count > this->workerPids[strWorkerName].size())
           {
               this->forkOneWorker(strWorkerName);
           }
       }
    }

    void Server::forkOneWorker(const string & strWorkerName)
    {
        int32_t sock[2];
        int32_t iRet = socketpair(AF_UNIX, SOCK_STREAM, 0, sock);
        if(iRet < 0)
        {
           printf("create socketpair fail\n");
           exit(114);
        }

        pid_t pid = fork();
        if(pid < 0)
        {
            printf("fork fail\n");
            exit(113);
        }
        else if(pid > 0)
        {
            close(sock[1]);
            this->channels.insert(pair<pid_t, uint32_t>(pid, sock[0]));
            this->setNonblock(this->channels[pid]);
            this->event.add(this->channels[pid], EV_READ, this, &Server::dealCmdResult, 0);
            if(this->workerPids.find(strWorkerName) == this->workerPids.end())
            {
                map<pid_t, pid_t> mapPid;
                mapPid.insert(pair<pid_t, pid_t>(pid, pid));
                this->workerPids.insert(pair<string, map<pid_t, pid_t> >(strWorkerName, mapPid));
            }
            this->workerPids[strWorkerName].insert(pair<pid_t, pid_t>(pid, pid));
            this->pidWorkers.insert(pair<pid_t, string>(pid, strWorkerName));
        }
        else
        {
            typename map<pid_t, int32_t>::iterator it;
            for(it = this->channels.begin(); it != this->channels.end(); it++)
            {
                close(it->second);
                this->event.del(it->second);
            }
            close(sock[0]);
            this->setNonblock(sock[1]);
            Worker objWorker(this->listenedSockets[strWorkerName]);
            objWorker.setChannel(sock[1]);
            objWorker.serve();
        }
    }

void Server::setNonblock(uint32_t fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK);
} 

void Server::dealCmdResult(uint32_t fd, uint16_t flag)
{
    char buf[1024];
    memset(buf, 0, 1024);
    int32_t iCount = recv(fd, buf, 1024, 0);
    if(iCount == 0)
    {
        cout<<"monitor workers\n";
        this->monitorWorkers();
    }   
}

void Server::monitorWorkers()
{
    int32_t iStatus;
    pid_t pid = waitpid(-1, &iStatus, WNOHANG | WUNTRACED);
    if(pid > 0)
    {
        printf("Worker exit pid:%d status:%d \n", pid, iStatus);
        this->clearWorker(pid);
        this->createWorkers();
    }
}

void Server::clearWorker(uint32_t pid)
{
    string strWorkerName = this->pidWorkers[pid];
    this->event.del(this->channels[pid]);
    this->channels.erase(pid);
    this->workerPids[strWorkerName].erase(pid);
    this->pidWorkers.erase(pid);
}

void Server::loop()
{
    this->event.loop();
}

const uint32_t Worker::MAX_BUF_SIZE = 1024;
const uint32_t Server::MAX_BUF_SIZE = 1024;

main ()
{
    Server objServer;
    objServer.run();
}
