//connect to a tcp server

#include "Connect.h"


int Connect(std::string& ip, int port){
    std::cout << "Connecting \n";


//addrinfo
    int fd;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    std::cout << hints.ai_family<<std::endl;
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    std::cout << "ip, port is ";

    char* tempIp = new char[ip.length() + 1];
    strcpy(tempIp, (ip).c_str());

    std::string int_string = std::to_string(port);
    const char * char_port = int_string.c_str();

    //port = htons(port);
    std::cout << "Connecting: initialised \n";
         std::cout << *tempIp <<" " << port<<std::endl;
    fd=getaddrinfo(tempIp, char_port, &hints, &servinfo);
    std::cout <<"fd is "<<fd<<std::endl;
    if(fd== -1){
        perror("getaddrinfo");
    }
//socket and connect
    int socket_res;
    int connect_res;
    for(p=servinfo; p!=NULL;p=p->ai_next){
        std::cout << "in for loop \n";
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
        break;
    }
    std::cout << "connected \n";

//recv
/*
    int recv_res;
    
    char message[500];
    recv_res = recv(socket_res, message, 100, 0);
    if(recv_res ==-1){
        perror("recv");
        return 0;
    }
    std::cout <<" size of message received: "<< recv_res <<std::endl;
    printf("message is ");
    //clear the space
    for(int i=0; i< sizeof message; i++){
        printf("%c", message[i]);
    }
    message[recv_res]='\0';
    */

//close socket later
    
    return socket_res;      
}