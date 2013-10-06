#include "Worker.h"

using namespace std;

void cb(uint32_t a, uint16_t b)
{
    cout<<"\nb="<<b<<"\n";;
}

Worker::Worker(int sock)
{
   this->mainSock = sock;
           
   fcntl(this->mainSock, F_SETFL, fcntl(this->mainSock, F_GETFD, 0)|O_NONBLOCK);
}

Worker::~Worker(){}

void Worker::serve()
{
    event.add(this->mainSock, EV_READ, this, &Worker::ccbb, 123);
    event.loop();
}

void Worker::ccbb(uint32_t fd, uint16_t flag)
{
    cout<<"fd:"<<fd<<" "<<"flag:"<<flag<<"\n";
}


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
