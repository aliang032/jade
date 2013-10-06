#include "Worker.h"

using namespace std;

Worker::Worker(int sock)
{
   this->mainSock = sock;
           
   fcntl(this->mainSock, F_SETFL, fcntl(this->mainSock, F_GETFD, 0)|O_NONBLOCK);
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
    uint32_t newConnection = accept(fd, (sockaddr *)(&stClientAddr), &sockLen);
    if(newConnection < 0)
    {
        return;
    }
    this->event.add(newConnection, EV_READ, this, &Worker::readFromConnection, 1000);
    char buf[Worker::MAX_BUF_SIZE];
    memset(buf, 0, Worker::MAX_BUF_SIZE);
    this->recvBuf.insert(pair<uint32_t, string>(newConnection, buf));
}

void Worker::readFromConnection(uint32_t fd, uint16_t flag)
{
    char buf[Worker::MAX_BUF_SIZE];
    memset(buf, 0, Worker::MAX_BUF_SIZE);
    uint32_t uiCount = read(fd, buf, Worker::MAX_BUF_SIZE);
    if(uiCount)
    {
        this->recvBuf[fd] += buf;
    }
    cout<<"count"<<uiCount<<" "<<this->recvBuf[fd]<<"\n";
    cout<<"read\n";
}

const uint32_t Worker::MAX_BUF_SIZE = 102400;

main ()
{
    int sock_fd;
    struct sockaddr_in my_addr;
    if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
       cout<<"socket create err\n";
       exit(110);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(3301);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), 0, 8);
    if(bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        cout<<"bind fail\n";
        exit(111);
    }
    if(listen(sock_fd, 10) == -1)
    {
        cout<<"listen fail\n";
        exit(112);
    }
    Worker worker(sock_fd);
    worker.serve();
}
