#ifndef REQUESTTRACKER_HPP
#define REQUESTTRACKER_HPP

#include <sstream> //for ostringstream
#include <string>
#include <iomanip> //for setw()
#include <iostream>
#include <random>
#include <vector>
 #include "loguru.hpp"
 #include "ParseTorrentFile.h"
// #include "PeerConnection.hpp"

std::string get_PeerID();
int bytesToInt(std::string bytes);

enum EVENT{
    STARTED, STOPPED, COMPLETED
};

class Peer{
    public:
    Peer(std::string ip, int port){
        this->ip = ip;
        this->port = port;
    }
    int peer_id;
    std::string ip;
    int port;
};


//is all these filds name sensitive?

class TrackerRequest{
    public:
    std::string announceUrl;
    std::string infohash;
    std::string get_PeerID();
    std::string peerId;
    int port;
    int uploaded;
    int downloaded;
    int left;
    int compact;
    EVENT event;
    std::vector <Peer *> peers;
    std::string TrackerResponse;
    std::string *filePath;
    std::string downloadPath;
    void get_peers(std::string& file_path);
    std::vector <Peer *> decodeResponse(std::string TrackerResponse );
    void decodePeers(const std::string& peers);
    void ConnectPeer();

    const TorrentFileParser& t_fileparser;
    TrackerRequest(const TorrentFileParser& parser): t_fileparser(parser) {}
//optional
/**
 * @brief 
 *  int IP;
    int numwant;
    int key;
    int trackerid;
 */

};
std::string hexDecode(const std::string& value);
/**
class TrackerResponse{
    std::string failure_reason;
    std::string warning_message;
    int interval;
    int min_interval;
    int reacker_id;
    int complete;
    int incomplete;
    std::vector <peers *> peer;
   
};
*/
#endif