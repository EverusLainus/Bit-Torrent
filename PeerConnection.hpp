#ifndef PEERCONNECTION_HPP
#define PEERCONNECTION_HPP

//Connect to the peers
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <cstdint> 
#include <iomanip>
#include "RequestTracker.hpp"
#include "BitTorrentMessage.hpp"
#include "PieceManager.hpp"
#include "ParseTorrentFile.h"
#include <poll.h>
#include <chrono>
#include "loguru.hpp"

class PeerConnection{
    public:
    
    std::string CreateHandshakeMessage();
    std::string infohash;
    std::string peerId;
    std::string peerBitField;
    int socket;
    bool choked;
    bool requestPending;
    std::string downloadPath;

    const TorrentFileParser& fileparser;
    
    PeerConnection(TorrentFileParser& parser, std::string downloadPath): fileparser(parser), downloadPath(downloadPath){}
    
    PieceManager* pieceManager = new PieceManager(fileparser, downloadPath);

    std::vector <Peer*> peers;
    int establishConnection();
    void PerformHandshake();
    void ReceiveBitfieldMessage(int sock);
    BitTorrentMessage ReceiveMessage(int socket);
    BitTorrentMessage receiveMessage(int bufferSize); 
    int SendInterested(int socket, BitTorrentMessage message);
    void requestPiece();
    void Start(int socket, BitTorrentMessage message);

    Peer* peer;

};

int bytesToInt(std::string bytes);

#endif