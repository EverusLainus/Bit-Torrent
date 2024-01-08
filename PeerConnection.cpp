#include "PeerConnection.hpp"
#include "RequestTracker.hpp"
#include "Connect.h"

std::string PeerConnection::CreateHandshakeMessage(){
    std::stringstream buffer;
    std::string protocol = "BitTorrent protocol";
    // Encode the length as a 32-bit integer in network byte order
    int len = protocol.length();
    std::cout <<"length of the buffer "<< len <<std::endl;
    //uint32_t protocolLength = htonl(protocol.length());
    
    //std::cout << "length of buffer:"<<protocolLength<<std::endl;
    
    std::string reserved_bytes;
    for(int i=0 ; i< 8; ++i){
        reserved_bytes.push_back('\0');
    }
    //const char *clen = (const char *) len;
    //std::cout <<"length of the buffer as char."<< clen<<std::endl;

    buffer <<(char) len;
    //buffer.write(reinterpret_cast<const char*>(&protocolLength), sizeof(protocolLength));
    std::cout <<"buffer ." <<buffer.str().size()<<std::endl;
    buffer << protocol;
    std::cout <<"buffer size after protocol:" <<buffer.str().size()<<std::endl;
    buffer << reserved_bytes;
    /*
    std::cout <<"buffer size after reserved bytes:" <<buffer.str().size()<<std::endl;
        for (char c : buffer.str()) {
        // Print each character in hexadecimal format
        std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
    }
    std::cout << std::endl;
    */
    std::cout<<"infohash:" <<infohash<<std::endl;
    buffer << infohash;
    std::cout <<"buffer size after infohash:" <<buffer.str().size()<<std::endl;
    std::cout<<" peetId:" <<peerId<<std::endl;
    buffer << peerId;
    std::cout <<"buffer ." <<buffer.str()<<std::endl;
    std::cout <<"buffer size after peerId:" <<buffer.str().size()<<std::endl;
    /*
        std::cout << "buffer: ";

    for (char c : buffer.str()) {
        // Print each character in hexadecimal format
        std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
    }
    std::cout << std::endl;
*/
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
    std::cout << "size is "<<size <<std::endl;
    std::cout <<"bytes read : "<<bytesRead<<std::endl;
    if(bytesRead == -1){
        perror("recv");
    }
    std::string reply;
    for(int i=0; i<bytesRead; ++i){
        reply.push_back(buffer[i]);
    }
    std::cout <<"received data "<<"..." << reply << std::endl;
    for(int i=0; i< reply.size(); ++i){
        std::cout << reply[i];
    }
    std::cout <<"\n";
}

void PeerConnection::PerformHandshake(){
    //get socket from Connect
    int socket = Connect(peer->ip, peer->port);
    std::cout << "PerformHandshake:"<< peerId<<", infohash:"<<infohash<<std::endl;
    std::string handshakeMessage = CreateHandshakeMessage();
    SendData(socket, handshakeMessage);
    std::string reply = ReceiveData(socket, handshakeMessage.length());
    std::cout <<"received data "<<reply <<std::endl;
}

