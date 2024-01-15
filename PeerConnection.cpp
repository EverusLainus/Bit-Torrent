#include "PeerConnection.hpp"
#include "Piece.hpp"

#include "Connect.h"

std::string PeerConnection::CreateHandshakeMessage(){
    std::cout << "createHandshakeMessage: in \n";
    std::stringstream buffer;
    std::string protocol = "BitTorrent protocol";
    // Encode the length as a 32-bit integer in network byte order
    int len = protocol.length();
    LOG_F(INFO, "CreateHandshakeMessage: length of the buffer %d ", len);
   //std::cout <<"CreateHandshakeMessage: length of the buffer "<< len <<std::endl;
    
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
    LOG_F(INFO, "CreateHandshakeMessage: buffer size after peerId:", buffer.str().size());

    return buffer.str();
}

void SendData(int sock, std::string& message){
    //std::cout << "sendData: in"<<std::endl;
    LOG_F(INFO, "sendData: in");
    int n= message.length();
    char buffer[n];
    //std::cout << "sendData: created buffer "<<std::endl;
    for(int i=0; i<n; ++i){
        buffer[i] = message[i];
    }
    LOG_F(INFO, "sendData: filled buffer");
    //std::cout << "sendData: filled buffer"<<std::endl;
    int bytes_send=0;
    int send_bytes;

    while(bytes_send < n){
        send_bytes = send(sock, buffer, n, 0);
        std::cout << send_bytes << std::endl;

        if(send_bytes < 0){
        perror("send");
        continue;
        }
        bytes_send += send_bytes;
    }
    
    //std::cout << "sendData: no of bytes send "<< send_bytes <<std::endl;
    LOG_F(INFO, "sendData: no of bytes send %d", send_bytes);
    
}

std::string  ReceiveData(int sock, int size){
    char buffer[size];
    std::string reply;

    struct pollfd pfd = { sock, POLLIN, 0};
    
    do{

    int ret = poll(&pfd, 1, -1);
    LOG_F(INFO, "ReceiveData: ret from poll %d", ret);
    //std::cout << "ret from poll "<< ret<<std::endl;
    if(ret == -1){
        perror("poll");
        continue;
    }
    if(pfd.revents & POLLIN){
    int bytesRead = recv(sock, buffer, size, 0);
    //std::cout << "ReceiveData:size is "<<size <<std::endl;
    //std::cout <<"ReceiveData: bytes read : "<<bytesRead<<std::endl;
    LOG_F(INFO, "ReceiveData: size is %d, bytes read %d", size, ret);
    if(bytesRead == -1){
        perror("recv");
        continue;
    }  
    size = size - bytesRead;
    for(int i=0; i<bytesRead; ++i){
        reply.push_back(buffer[i]);
    }
    }
    }while (size > 0);
    
    //std::cout <<"ReceiveData: received data "<<"." << reply<<". " << std::endl;
    LOG_F(INFO, "ReceiveData: received data :.%s.", reply.c_str());
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
    } else if( ret == 0){
        perror("poll");
    }
     
    int lengthofMessage = 4;
    char buffer[lengthofMessage];
    long bytesRead = recv(sock, buffer, sizeof(buffer), 0);
    LOG_F(INFO, "ReceiveBitfieldMessage: bytes read:%d", bytesRead);
    //std::cout << "ReceiveBitfieldMessage: bytes read:"<<bytesRead << std::endl;
    if (bytesRead != lengthofMessage){
       // std::cout << "ReceiveBitfieldMessage: length is four "<<std::endl;
       LOG_F(INFO, "ReceiveBitfieldMessage: length is four");
    }

    std::string messageLengthStr;
    for (char i : buffer)
        messageLengthStr += i;
    uint32_t messageLength = bytesToInt(messageLengthStr);
    int bufferSize = messageLength;
    LOG_F(INFO, "ReceiveBitfieldMessage: message is: %s, length of buffer: %d", messageLengthStr.c_str(), bufferSize);
    //std::cout << "ReceiveBitfieldMessage: message is: "<<messageLengthStr <<std::endl;
    //std::cout << "ReceiveBitfieldMessage: length of the buffer: "<<bufferSize <<std::endl;
    std::string receive_bitField_message = ReceiveData(sock, bufferSize);
    if (receive_bitField_message.empty())
        BitTorrentMessage(keepAlive);
    auto messageId = (uint8_t) receive_bitField_message[0];
    LOG_F(INFO, "ReceiveBitfieldMessage: message id is %d", messageId);
    //printf("message id is %d\n", messageId);
    //std::cout << "message id is :" << messageId << std::endl;
    std::string payload = receive_bitField_message.substr(1);
    BitTorrentMessage message(messageId, payload);
    //printf("message id is %d\n", message.message_id);
    LOG_F(INFO, "ReceiveBitfieldMessage: message id is %d", message.message_id);
    if(message.message_id != bitField){
        LOG_F(INFO, "ReceiveBitfieldMessage: expected a bitField message");
        //std::cout << " ReceiveBitfieldMessage: expected a bitField message\n";
    }
    peerBitField = message.bitfield;
    //printf("bitfield message in printf is %d\n", message.bitfield.c_str());
    LOG_F(INFO, "ReceiveBitfieldMessage: bitfield message in printf is %s", message.bitfield.c_str());
    std::cout << "bitfield message is "<<peerBitField << std::endl;
    LOG_F(INFO, "ReceiveBitfieldMessage: bitfield message is %s", peerBitField.c_str());
    SendInterested(sock, message);
}

void PeerConnection::PerformHandshake(){
    //get socket from Connect
    int socket = Connect(peer->ip, peer->port);
    LOG_F(INFO, "PerformHandshake: with peerId %s, infohash %s", peerId.c_str(), infohash.c_str());
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
        LOG_F(INFO, "PerformHandshake: hashes match ");
        //std::cout << "ReceiveBitfieldMessage: hashes match with res "<< res <<std::endl;
    }
    std::cout <<"PerformHandshake:received data "<<reply <<std::endl;
    LOG_F(INFO, "PerformHandshake: received data %s ", reply.c_str());
    ReceiveBitfieldMessage(socket);
}

int PeerConnection::SendInterested(int socket, BitTorrentMessage message ){

    std::string interestedMessage;
    interestedMessage= BitTorrentMessage(interested).toString();
    SendData(socket, interestedMessage);
    std::cout << "interested message send  " << std::endl;
    LOG_F(INFO, "SendInterested: interested message send");
    Start(socket, message);

    return 1;
}

BitTorrentMessage PeerConnection::ReceiveMessage(int socket){
    //std::cout << "ReceiveMessage: in" << std::endl;
    LOG_F(INFO, "ReceiveMessage: in");

    struct pollfd fd;
    fd.fd = socket;
    fd.events = POLLIN;
    int ret = poll(&fd, 1, 3000);
    if(ret == -1){
        perror("poll");
    } else if( ret == 0){
        perror("poll");
    }
     
    int lengthofMessage = 4;
    char buffer[lengthofMessage];
    long bytesRead;
    int read;
    do{
        
    bytesRead = recv(socket, buffer, sizeof(buffer), 0);
    if(bytesRead < 0){
        continue;
    }
    read += bytesRead;
    }while(read < 4);

    LOG_F(INFO, "ReceiveMessage: bytes read: %d", bytesRead);
    //std::cout << "ReceiveMessage: bytes read:"<<bytesRead << std::endl;
    if (bytesRead != lengthofMessage){
        LOG_F(INFO, "ReceiveMessage: length is four", bytesRead);
        //std::cout << "ReceiveMessage: length is four "<<std::endl;
    }

    std::string messageLengthStr;
    for (char i : buffer)
        messageLengthStr += i;
    uint32_t messageLength = bytesToInt(messageLengthStr);
    int bufferSize = messageLength;
    //std::cout << "ReceiveMessage: message length is: "<< bufferSize <<std::endl;
    LOG_F(INFO, "ReceiveMessage: message length is: %d", bufferSize);

    
    std::string reply = ReceiveData(socket, bufferSize);
    if (reply.empty())
        return BitTorrentMessage(keepAlive);
    auto messageId = (uint8_t) reply[0];
    LOG_F(INFO, "ReceiveMessage: with id %d", messageId);
    //printf("receiveMessage: with id %d\n", messageId);
    std::string payload = reply.substr(1);
    return BitTorrentMessage(messageId, payload);
}



void PeerConnection::Start(int socket, BitTorrentMessage message){
    LOG_F(INFO, "Start: in ");
    //std::cout << "Start: in "<<std::endl;
    BitTorrentMessage receive_msg;
    this->socket = socket;
   // std::cout << "declared a Bittorrent message variable \n";
    if(1){
        
        receive_msg = ReceiveMessage(socket);
       // std::cout << " send interested is true with id "<< receive_msg.message_id << std::endl;
        LOG_F(INFO, "Start: send interested is true with id %u ", receive_msg.message_id);
    
    switch(receive_msg.message_id){
        case choke:
            choked = true;
            std::cout << "choke"<<std::endl;
            LOG_F(INFO, "Start: peer choked us");
            break;

        case unchoke:
            choked = false;
            std::cout << "unchoke"<<std::endl;
            LOG_F(INFO, "Start: peer unchoked us");
            break;

        case piece:
        {
            std::string payload = receive_msg.getPayload();
            int index = bytesToInt(payload.substr(0, 4));
            int begin = bytesToInt(payload.substr(4, 4));
            std::string blockData = payload.substr(8);

            LOG_F(INFO, "Start: block of index %d beginning at %d is received", index, begin);
            std::cout << "block of index " << index <<
            " beginning at " << begin << "received " << std::endl;
            break;
        }
        default:
         LOG_F(INFO, "Start: error with id %d ", receive_msg.message_id);
            std::cout << "error with id "<<receive_msg.message_id << std::endl;
    }
    }
    if (!choked)//unchoked
    {
        //if request is not pending
        if (1)
        {
            std::cout << "request piece \n";
        requestPiece();
        }
    }
}

/*
void PeerConnection::requestPiece(){
    //instead of getting block from piece manager
    //i have created a block
    Block block;
    block.piece =1;
    block.offset = 1;
    block.length = 16384; //2^14 bytes

     //* @brief request message has
     //* length(4) messagetype (1) Piece index (4) Block offset (4) Block Length (4)

    std::stringstream requestPeice;
    int length_of_message = 17;
    //requestPeice.write(reinterpret_cast<char*>(&length_of_message), sizeof(int));
    int message_type = 6;
    //requestPeice.write(reinterpret_cast<char*>(&message_type), sizeof(char));
    int PieceIndex = block.piece;
    requestPeice.write(reinterpret_cast<char*>(&PieceIndex), sizeof(int));
    int blockOffset = block.offset;
    requestPeice.write(reinterpret_cast<char*>(&blockOffset), sizeof(int));
    int blockLength = block.length;
    requestPeice.write(reinterpret_cast<char*>(&blockLength), sizeof(int));
    //requestPeice << length_of_message;
    //requestPeice << (char) message_type;
    //requestPeice << blockOffset;
    //requestPeice << blockLength;
    std::string payload = requestPeice.str();
    
    std::string request_msg  = BitTorrentMessage(request, payload).toString();
    //std::cout << "request:" << request<<" with size:" << request.size() <<std::endl;
    printf("interest request: %s", request_msg.c_str());
    LOG_F(INFO, "requestPiece: message %s ", request_msg.c_str());
    int sock = this->socket;
    SendData(sock, request_msg);
    BitTorrentMessage request_reply = ReceiveMessage(sock);
    Start(sock, request_reply);

}

*/

void PeerConnection::requestPiece() {

    Block block;
    block.piece =1;
    block.offset = 1;
    block.length = 16384; //2^14 bytes

    int payloadLength = 12;
    char temp[payloadLength];
    // Needs to convert little-endian to big-endian
    uint32_t index = htonl(block.piece);
    uint32_t offset = htonl(block.offset);
    uint32_t length = htonl(block.length);
    std::memcpy(temp, &index, sizeof(int));
    std::memcpy(temp + 4, &offset, sizeof(int));
    std::memcpy(temp + 8, &length, sizeof(int));
    std::string payload;
    for (int i = 0; i < payloadLength; i++)
        payload += (char) temp[i];

    std::stringstream info;
    info << "Sending Request message to peer " << peer->ip << " ";
    info << "[Piece: " << std::to_string(block.piece) << " ";
    info << "Offset: " << std::to_string(block.offset) << " ";
    info << "Length: " << std::to_string(block.length) << "]";
    LOG_F(INFO, "%s", info.str().c_str());
    std::string requestMessage = BitTorrentMessage(request, payload).toString();
    std::cout << "request Message: message is "<<requestMessage<<std::endl;
    printf("request message: %s", requestMessage.c_str());
    int sock = this->socket;
    SendData(sock, requestMessage);
    //requestPending = true;
    LOG_F(INFO, "Send Request message: SUCCESS");
}