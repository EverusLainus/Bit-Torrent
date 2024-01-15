//connect to a tcp server

#include "Connect.h"
#include <fcntl.h>

bool setSocketBlocking(int sock, bool blocking)
{
    if (sock < 0)
        return false;

    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(sock, F_SETFL, flags) == 0);
}

int Connect(std::string& ip, int port){
    LOG_F(INFO, "going to Connect to peer with, Port:%d, Ip:%s ", port, ip.c_str());


//addrinfo
    int fd;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    std::cout << hints.ai_family<<std::endl;
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    char* tempIp = new char[ip.length() + 1];
    strcpy(tempIp, (ip).c_str());

    std::string int_string = std::to_string(port);
    const char * char_port = int_string.c_str();

    fd=getaddrinfo(tempIp, char_port, &hints, &servinfo);
    if(fd== -1){
        perror("getaddrinfo");
    }
//socket and connect
    int socket_res;
    int connect_res;
    for(p=servinfo; p!=NULL;p=p->ai_next){
        //std::cout << "Connect: in for loop \n";
        socket_res= socket(p->ai_family, p->ai_socktype, 0);
        if (socket_res == -1){
            perror("socket");
            continue;
        }
        connect_res=connect(socket_res,p->ai_addr,p->ai_addrlen );
        if(connect_res== -1){
            perror("connect");
            continue;
        }
        else{
            LOG_F(INFO, "Connected to peer, Port:%d, Ip:%s ", port, ip.c_str());
        }
        break;
    }
    // Sets socket to non-block mode
    if (!setSocketBlocking(socket_res, false)){
        perror("nonblocking");
        return -1;
    }
   
    return socket_res;      
}