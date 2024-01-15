//Connect to the peers
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <cstdint> 
#include <iomanip>
#include "RequestTracker.hpp"
#include "BitTorrentMessage.hpp"
#include <poll.h>
#include "loguru.hpp"

class PeerConnection{
    public:
    
    std::string CreateHandshakeMessage();
    std::string infohash;
    std::string peerId;
    std::string peerBitField;
    int socket;
    bool choked;

    
    
    std::vector <Peer*> peers;
    void PerformHandshake();
    void ReceiveBitfieldMessage(int sock);
    BitTorrentMessage ReceiveMessage(int socket);
    int SendInterested(int socket, BitTorrentMessage message);
    void requestPiece();
    void Start(int socket, BitTorrentMessage message);

    Peer* peer;

};