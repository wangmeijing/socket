#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory.h>
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
        cout << "creat socket failed" << endl;
        return -1;
    }

    int on = 1;
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        cout << "setsocketopt failed" << endl;
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind between client and server
    if(bind(st, (struct sockaddr *) &addr, sizeof(addr))  == -1)
    {
        cout << "bind failed" << endl;
        return -1;
    }

    //server listen from client
    if(listen(st, 20) == -1)
    {
        cout << "listen failed" << endl;
        return -1;
    }

//    cout << "server start work........" <<endl;

//    char s[1024];
    int client_st = 0;
    struct sockaddr_in client_addr;

    for(int  i = 0; i < 5; i++)
    {
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);

        client_st = accept(st, (struct sockaddr *) &client_addr, &len);
        if(client_st == -1)
        {
            cout << "accept failed" << strerror(errno) << endl;
            return -1;
        }

        cout <<"accept by " << inet_ntoa(client_addr.sin_addr) << endl;

        //1....
//        memset(s, 0, sizeof(1024));

//        if(recv(client_st, s, sizeof(s), 0) == -1)
//        {
//            cout << "recv failed" << strerror(errno) << endl;
//            close(client_st);
//            return -1;
//        }

//        cout << "recv is " << s <<endl;
//        close(client_st);

//        while (1) {
//            memset(s,0,sizeof(1024));

//            int rc = recv(client_st, s, sizeof(s), 0);
//            if( rc > 0)
//            {
//                cout <<"recv is " << s <<endl;
//                memset(s, 0, sizeof(s));
//                read(STDIN_FILENO, s, sizeof(s));
//                send(client_st, s, strlen(s),0);
//            }
//            else
//            {
//                if(rc == 0)
//                    cout << "client socket colsed." << endl;
//                else
//                    cout <<" recv failed " <<strerror(errno) <<endl;

//                break;
//            }
//        }

        std::thread sendThread(sendsocket, client_st);
        std::thread recvThread(recvsocket, client_st);

        sendThread.detach();
        recvThread.detach();

//        sendThread.join();
//        recvThread.join();

//        close(client_st);
    }

    close(st);
    return 0;
}

