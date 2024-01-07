//Connect to the peers
#include <string>
#include <sstream>
#include "RequestTracker.hpp"

class PeerConnection{
    public:
    
    std::string CreateHandshakeMessage();
    std::string infohash;
    std::string peerId;
    
    std::vector <Peer*> peers;
    void PerformHandshake();

    Peer* peer;

};