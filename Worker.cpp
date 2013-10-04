#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
class Worker
{
    private:
        int mainSock;
    public:
        Worker(int sock)
        {
           this->mainSock = sock;
           cout<<this->mainSock<<"\n";
        }
};

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
    while(1){} 
}
