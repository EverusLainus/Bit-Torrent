#include "PeerConnection.hpp"
#include "RequestTracker.hpp"
#include "Connect.h"

std::string PeerConnection::CreateHandshakeMessage(){
    std::stringstream buffer;
    std::string protocol = "BitTorremt protocol";
    
        std::string reserved_bytes;
    for(int i=0 ; i< 8; ++i){
        reserved_bytes.push_back('\0');
    }

    buffer << (char) protocol.length();
    buffer << protocol;
    buffer << reserved_bytes;
    buffer << hexDecode( infohash );
    buffer << peerId;

    return buffer.str();
}

void SendData(int sock, std::string& message){
    std::cout << "sendData: in"<<std::endl;
    int n= message.length();
    char buffer[n];
    for(int i=0; i<n; ++i){
        buffer[i] = message[i];
    }
    int send_bytes = send(sock, buffer, n, 0);
    std::cout << "sendData: no of bytes send "<< send_bytes <<std::endl;
    if(send_bytes < 0){
        perror("send");
    }
}

std::string  ReceiveData(int sock, int size){
    char buffer[size];
    int bytesRead = recv(sock, buffer, size, 0);
    std::string reply;
    std::cout <<"received data "<<"..."<<reply <<std::endl;
}

void PeerConnection::PerformHandshake(){
    //get socket from Connect
    int socket = Connect(peer->ip, peer->port);
    std::string handshakeMessage = CreateHandshakeMessage();
    SendData(socket, handshakeMessage);
    std::string reply = ReceiveData(socket, handshakeMessage.length());
    std::cout <<"received data "<<reply <<std::endl;
}

