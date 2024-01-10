#include "PeerConnection.hpp"
#include "RequestTracker.hpp"
#include "Connect.h"

std::string PeerConnection::CreateHandshakeMessage(){
    std::stringstream buffer;
    std::string protocol = "BitTorrent protocol";
    // Encode the length as a 32-bit integer in network byte order
    int len = protocol.length();
    std::cout <<"CreateHandshakeMessage: length of the buffer "<< len <<std::endl;
    
    std::string reserved_bytes;
    for(int i=0 ; i< 8; ++i){
        reserved_bytes.push_back('\0');
    }

    buffer <<(char) len;
    
    //std::cout <<"buffer ." <<buffer.str().size()<<std::endl;
    buffer << protocol;
    //std::cout <<"buffer size after protocol:" <<buffer.str().size()<<std::endl;
    buffer << reserved_bytes;
    buffer << infohash;
    //std::cout <<"buffer size after infohash:" <<buffer.str().size()<<std::endl;
    //std::cout<<" peetId:" <<peerId<<std::endl;
    buffer << peerId;
    std::cout <<"CreateHandshakeMessage: buffer size after peerId:" <<buffer.str().size()<<std::endl;

    return buffer.str();
}

void SendData(int sock, std::string& message){
    std::cout << "sendData: in"<<std::endl;
    int n= message.length();
    char buffer[n];
    std::cout << "sendData: created buffer "<<std::endl;
    for(int i=0; i<n; ++i){
        buffer[i] = message[i];
    }
    std::cout << "sendData: filled buffer"<<std::endl;
    int send_bytes = send(sock, buffer, n, 0);
    std::cout << "sendData: after send "<<std::endl;
    std::cout << "sendData: no of bytes send "<< send_bytes <<std::endl;
    if(send_bytes < 0){
        perror("send");
    }
}

std::string  ReceiveData(int sock, int size){
    char buffer[size];
    int bytesRead = recv(sock, buffer, size, 0);
    std::cout << "ReceiveData:size is "<<size <<std::endl;
    std::cout <<"ReceiveData: bytes read : "<<bytesRead<<std::endl;
    if(bytesRead == -1){
        perror("recv");
    }
    std::string reply;
    for(int i=0; i<bytesRead; ++i){
        reply.push_back(buffer[i]);
    }
    std::cout <<"ReceiveData: received data "<<"." << reply<<". " << std::endl;
    /*
    for(int i=0; i< reply.size(); ++i){
        std::cout<< i <<"th value is " << reply[i]<<std::endl;
    }
    */
    return reply;  
}

void printHex(const std::string& str) {
    for (char c : str) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << std::endl;
}

/**
 * Converts a series of bytes in a string format to an integer.
 */
int bytesToInt(std::string bytes)
{
    // FIXME: Use bitwise operation to convert
    std::string binStr;
    long byteCount = bytes.size();
    for (int i = 0; i < byteCount; i++)
        binStr += std::bitset<8>(bytes[i]).to_string();
    return stoi(binStr, 0, 2);
}

void PeerConnection::ReceiveBitfieldMessage(int sock){
    //wait for incoming data
    struct pollfd fd;
    fd.fd = sock;
    fd.events = POLLIN;
    int ret = poll(&fd, 1, 3000);
    if(ret == -1){
        perror("poll");
    } else if( ret = 0){
        perror("poll");
    }
     
    int lengthofMessage = 4;
    char buffer[lengthofMessage];
    long bytesRead = recv(sock, buffer, sizeof(buffer), 0);
    std::cout << "ReceiveBitfieldMessage: bytes read:"<<bytesRead << std::endl;
    if (bytesRead != lengthofMessage){
        std::cout << "ReceiveBitfieldMessage: length is four "<<std::endl;
    }

    std::string messageLengthStr;
    for (char i : buffer)
        messageLengthStr += i;
    uint32_t messageLength = bytesToInt(messageLengthStr);
    int bufferSize = messageLength;
    std::cout << "ReceiveBitfieldMessage: message is: "<<messageLengthStr <<std::endl;
    std::cout << "ReceiveBitfieldMessage: length of the buffer: "<<bufferSize <<std::endl;
    std::string receive_bitField_message = ReceiveData(sock, bufferSize);
    auto messageId = (uint8_t) receive_bitField_message[0];
}

void PeerConnection::PerformHandshake(){
    //get socket from Connect
    int socket = Connect(peer->ip, peer->port);
    std::cout << "PerformHandshake:"<< peerId<<", infohash:"<<infohash<<std::endl;
    std::string handshakeMessage = CreateHandshakeMessage();
    SendData(socket, handshakeMessage);
    std::string reply = ReceiveData(socket, handshakeMessage.length());
    peerId = reply.substr(48, 20);
    std::string receivedInfohash = reply.substr(28, 20);
    if(receivedInfohash == infohash){
        //std::cout <<"\nReceiveBitfieldMessage:  comparing strings "
        //<< receivedInfohash << " and " << infohash << std::endl;
        int res = (receivedInfohash == infohash);
        std::cout << "ReceiveBitfieldMessage: hashes match with res "<< res <<std::endl;
    }
    std::cout <<"ReceiveBitfieldMessage: received data "<<reply <<std::endl;
    ReceiveBitfieldMessage(socket);
}

