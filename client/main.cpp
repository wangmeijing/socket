#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <thread>

using namespace std;

void recvsocket(int st)
{
    int socket_num = st;
    char s[1024];

    while(1)
    {
       memset(s, 0, sizeof(s));
       int rc = recv(socket_num, s, sizeof(s), 0);
       if(rc <= 0)
           break;
       cout << s <<endl;
    }
}


void sendsocket(int st)
{
    int socket_num = st;
    char s[1024];

    while(1)
    {
        memset(s, 0, sizeof(s));
        read(STDIN_FILENO, s, sizeof(s));
        send(socket_num, s, sizeof(s), 0);
    }
}

int main(void)
{
    int st = socket(AF_INET, SOCK_STREAM, 0);
    if(st == -1)
    {
        cout << "creat socket failed." << strerror(errno) << endl;
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("192.168.253.22");

    if(connect(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
        cout << "connect failed "  << strerror(errno) << endl;
        return -1;
    }

//    while(1)
//    {
//        char s[1024];
//        memset(s, 0, sizeof(1024));
//        read(STDIN_FILENO, s, sizeof(s));
//        if(send(st, s, sizeof(s), 0) == -1)
//        {
//            cout << "send failed " << strerror(errno) << endl;
//            return -1;
//        }

//        memset(s,0,sizeof(s));
//        if(recv(st, s, sizeof(s),0) > 0)
//            cout << s << endl;
//        else
//            break;
//    }

    std::thread sendThread(sendsocket, st);
    std::thread recvThread(recvsocket, st);

    sendThread.join();
    recvThread.join();

    close(st);

    return 0;
}

