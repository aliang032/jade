#include "Worker.h"

using namespace std;

Worker::Worker(int sock)
{
   this->mainSock = sock;
   this->setNonblock(this->mainSock);        
}

Worker::~Worker(){}

void Worker::serve()
{
    this->event.add(this->mainSock, EV_ACCEPT, this, &Worker::acceptConnection, 0);
    this->event.loop();
}

void Worker::acceptConnection(uint32_t fd, uint16_t flag)
{
    socklen_t sockLen = sizeof(struct sockaddr_in);
    struct sockaddr_in stClientAddr;
    int32_t newConnection = accept(fd, (sockaddr *)(&stClientAddr), &sockLen);
    if(newConnection <= 0)
    {
        return;
    }
    connectionItem stConItem;
    this->connections.insert(pair<uint32_t, connectionItem>(newConnection, stConItem));
    this->connections[newConnection].fd = newConnection;
    this->connections[newConnection].recv_buf = "";
    this->setNonblock(this->connections[newConnection].fd);
    this->event.add(this->connections[newConnection].fd, EV_READ, this, &Worker::baseDealInput, 1000);
}

void Worker::baseDealInput(uint32_t fd, uint16_t flag)
{
    this->currentConnection = fd;
    char cBuf[Worker::MAX_BUF_SIZE + 1];
    memset(cBuf, 0, Worker::MAX_BUF_SIZE + 1);
    int32_t iCount = recv(fd, cBuf, Worker::MAX_BUF_SIZE, 0);
    if(iCount > 0)
    {
        this->connections[fd].recv_buf += cBuf;
    }
    else if((iCount < 0 && EAGAIN == errno) || iCount == 0)
    {
        printf("@@@@@@@@@@@@@@@@@@@@@@@@readFromConnection Fail errno:%d@@@@@@@@@@@@@@@@@@@@@@@@@@@@", errno);
        if(EAGAIN == errno)
        {
            this->closeConnection(fd);
        }
    }
    //cout<<"read:count"<<iCount<<" "<<this->connections[fd].recv_buf<<"\n";
    int32_t iRemainLength = this->dealInput(this->connections[fd].recv_buf);
    if(iRemainLength == 0)
    {
        this->dealProcess(this->connections[fd].recv_buf);
        this->closeConnection(fd);
    }
    else if(iRemainLength < 0)
    {
        printf("iRemainLength:%d<0", iRemainLength);
        this->closeConnection(fd);
    }
}

int32_t Worker::dealInput(string &strData)
{
    return 0;
}

void Worker::dealProcess(string &strData)
{
   this->sendToClient(strData);
}

int32_t Worker::sendToClient(const string &strData)
{
    int32_t iCount = send(this->currentConnection, strData.c_str(), strData.size(), 0);
    return iCount;
}

int32_t Worker::sendToClient(string* strData)
{
    int32_t iCount = send(this->currentConnection, strData->c_str(), strData->size(), 0);
    return iCount;
}

void Worker::closeConnection(uint32_t fd)
{
    this->event.del(fd);
    if(this->connections.find(fd) != this->connections.end())
    {
        this->connections.erase(fd);
        close(fd);
    }    
}

void Worker::setChannel(uint32_t uiChannel)
{
    this->channel = uiChannel;
    this->setNonblock(this->channel);
    this->event.add(this->channel, EV_READ, this, &Worker::dealCmd, 0);
}

void Worker::dealCmd(uint32_t fd, uint16_t flag)
{
    char buf[1024];
    memset(buf, 0, 1024);
    int32_t iCount = recv(fd, buf, 1024, 0);
    if(iCount == 0)
    {
        cout<<"<<<<<<<<<<<<<<<<<<<<master has gone ??? fd:"<<fd<<">>>>>>>>>>>>>>>>>>>>\n";
        this->event.del(fd);
    }
    else
    {
        cout<<"dealCmd:"<<buf<<"\n";
    }
}

void Worker::setNonblock(uint32_t fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK);
}

