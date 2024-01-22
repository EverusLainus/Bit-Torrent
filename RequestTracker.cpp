//give request to the tracker
#include <sstream> //for ostringstream
#include <string>
#include <iomanip> //for setw()
#include <iostream>
#include <random>
#include "ParseTorrentFile.h"
#include "RequestTracker.hpp"
#include "cpr/cpr.h"
#include "./cpp-bencoding/include/bencoding.h"
#include "Connect.h"
#include "PeerConnection.hpp"
#define TRACKER_TIMEOUT 15000

//get announce URL
std::string TrackerRequest::get_PeerID(){ 
    std::cout << "get_PeerID: into peers ID"<<std::endl;
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(20);
    for(size_t i=0; i<20 ; ++i){
        result.push_back(charset[dist(rng)]);
    }
    std::cout <<"get_PeerID: peerId : "<< result <<std::endl;
    // Create an output stream to build the encoded string
    std::ostringstream encoded;

    // Set the fill character for padding to '0'
    encoded.fill('0');

    // Set the output stream to hexadecimal mode
    encoded << std::hex;

    // Iterate over each character in the input string
    for (char c : result) {
    
        // Keep alphanumeric characters and some special characters unencoded
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;  // Append unencoded character to the output stream
        } else {
            // Encode non-alphanumeric characters
            encoded << std::uppercase << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
            
            // ^ Convert character to its ASCII code in hexadecimal and append to the output stream
        }
    }
    std::cout <<"get_PeerID: urlencoded peerId : "<< encoded.str()<<std::endl;

    // Convert the output stream to a string and return
    return encoded.str();
}

std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // Keep alphanumeric characters and other allowed characters as-is
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            // Convert special characters to %xx format
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }
    return escaped.str();
}

/**
 * Converts a string in hexadecimal form to a string in char form.
 * Implementation from https://stackoverflow.com/questions/3790613/how-to-convert-a-string-of-hex-values-to-a-string.
 */
std::string hexDecode(const std::string& value)
{
    int hashLength = value.length();
    std::string decodedHexString;
    for (int i = 0; i < hashLength; i += 2)
    {
        std::string byte = value.substr(i, 2);
        char c = (char) (int) strtol(byte.c_str(), nullptr, 16);
        decodedHexString.push_back(c);
    }
    std::cout <<"hexDecode: decoded hex string:"<< decodedHexString<<std::endl;
    return decodedHexString;
}



//assumes someone has filled the tracker Request class
void TrackerRequest::get_peers(std::string& file_path){
    std::cout << "get_peers: into get_peers "<<std::endl;
    filePath = &file_path;
    std::cout << "filePath: "<<*filePath<<std::endl;
    TorrentFileParser torrentFileParser(file_path);
    infohash =torrentFileParser.getInfoHash();
    //peerId = TrackerRequest::get_PeerID();
    announceUrl = torrentFileParser.getAnnounce();
    std::cout << "announceUrl: "<< announceUrl << std::endl;

    std::string info = torrentFileParser.getInfoHash();

    std::string peerid = get_PeerID();

    port = 2300;
    int fileSize = torrentFileParser.getFileSize();

    std::string url_encoded_hashInfo = urlEncode(info);
    infohash = hexDecode(url_encoded_hashInfo);
    std::cout << "get peers: url encoded hash "<< url_encoded_hashInfo <<std::endl;
    std::cout << "get peers: hex decode  "<< infohash <<std::endl;

    cpr::Response res; 
    try{
    res = cpr::Get(cpr::Url{announceUrl}, cpr::Parameters {
            { "info_hash", std::string(infohash) },
            { "peer_id", std::string(peerid) },
            { "port", std::to_string(port) },
            { "uploaded", std::to_string(0) },
            { "downloaded", std::to_string(0) },
            { "left", std::to_string(fileSize - 0) },
            { "compact", std::to_string(0) }
        }, cpr::Timeout{ TRACKER_TIMEOUT }
    );
    }catch (const std::length_error& e){
        std::cerr << "Length error: " << e.what() << std::endl;
    }
    //check if the request was successful
    if(res.status_code == 200){
        std::cout <<"Tracker response: "<<res.text <<std::endl;
        
        std::shared_ptr<bencoding::BItem> decodedResponse = bencoding::decode(res.text);
            if (auto dictionary = std::dynamic_pointer_cast<bencoding::BDictionary>(decodedResponse)) {
    std::string prettyRepr = bencoding::getPrettyRepr(decodedResponse);
    std::cout << prettyRepr << std::endl;
            }
            // Access and print the parsed information
            
            
    }else{
        std::cerr <<"get_peers: Tracker request failed with status code: "<<res.status_code<<std::endl;

            for (const auto& header : res.header) {
                std::cout << header.first << ": " << header.second << std::endl;
            }

            // Print the response body
            std::cout << "Response Body: " << res.text << std::endl;
    }
    TrackerResponse = res.text;
}

//to decode peers in compact form
void TrackerRequest::decodePeers(const std::string& peer) {
    if (peer.size() % 6 != 0) {
        std::cerr << "Invalid peers data length." << std::endl;
        return;
    }

    for (size_t i = 0; i < peer.size(); i += 6) {
        // Extract the 4-byte IP address and 2-byte port
        uint32_t ipAddress;
        uint16_t port;
        std::memcpy(&ipAddress, peer.data() + i, 4);
        std::memcpy(&port, peer.data() + i + 4, 2);
        std::string peerIp = std::to_string(ipAddress);
        int peerPort = static_cast<int>(port);
        // Convert network byte order to host byte order
        ipAddress = ntohl(ipAddress);
        port = ntohs(port);

        // Print the decoded peer information
        std::cout << "Peer: " << inet_ntoa(*(struct in_addr*)&ipAddress) << ":" << port << std::endl;
        Peer* newPeer = new Peer(peerIp, peerPort);
        peers.push_back(newPeer);
    }
}

std::vector <Peer *> TrackerRequest::decodeResponse(std::string response){
    std::shared_ptr<bencoding::BItem> decodedResponse;
    try{
    decodedResponse = bencoding::decode(response);
    } catch(const bencoding::DecodingError& e){
        std::cerr << "Caught DecodingError: " << e.what() << std::endl;
    }
    std::shared_ptr<bencoding::BDictionary> responseDict =
            std::dynamic_pointer_cast<bencoding::BDictionary>(decodedResponse);
        std::shared_ptr<bencoding::BItem> peersValue = responseDict->getValue("peers");
            //peer info is stored in a list


            if(typeid(*peersValue) == typeid(bencoding::BList)){
                 std::shared_ptr<bencoding::BList> peerList = std::dynamic_pointer_cast<bencoding::BList>(peersValue);
                 for(auto &item : *peerList){
                    //each item is a dictionar
                    std::shared_ptr<bencoding::BDictionary> peerDict = std::dynamic_pointer_cast<bencoding::BDictionary>(item);

                    std::shared_ptr<bencoding::BItem> tempPeerIp = peerDict->getValue("ip");
                    std::string peerIp = std::dynamic_pointer_cast<bencoding::BString>(tempPeerIp)->value();
                    std::shared_ptr<bencoding::BItem> tempPeerPort = peerDict->getValue("port");
                    int peerPort = (int) std::dynamic_pointer_cast<bencoding::BInteger>(tempPeerPort)->value();
                    Peer* newPeer = new Peer(peerIp, peerPort);
                    peers.push_back(newPeer);
                }
            
            std::cout <<"decodeResponse: peers size "<< peers.size() <<std::endl;
    }
    else{
        if (typeid(*peersValue) == typeid(bencoding::BString))
            {
                // Unmarshalls the peer information:
                // Detailed explanation can be found here:
                // https://blog.jse.li/posts/torrent/
                // Essentially, every 6 bytes represent a single peer with the first 4 bytes being the IP
                // and the last 2 bytes being the port number.
                const int peerInfoSize = 6;
                std::string peersString = std::dynamic_pointer_cast<bencoding::BString>(peersValue)->value();

                if (peersString.length() % peerInfoSize != 0)
                    throw std::runtime_error(
                            "Received malformed 'peers' from tracker. ['peers' length needs to be divisible by 6]");

                const int peerNum = peersString.length() / peerInfoSize;
                for (int i = 0; i < peerNum; i++)
                {
                    int offset = i * peerInfoSize;
                    std::stringstream peerIp;
                    peerIp << std::to_string((uint8_t) peersString[offset]) << ".";
                    peerIp << std::to_string((uint8_t) peersString[offset + 1]) << ".";
                    peerIp << std::to_string((uint8_t) peersString[offset + 2]) << ".";
                    peerIp << std::to_string((uint8_t) peersString[offset + 3]);
                    int peerPort = bytesToInt(peersString.substr(offset + 4, 2));
                    // std::cout << "IP: " << peerIp.str() << std::endl;
                    // std::cout << "Port: " << std::to_string(peerPort) << std::endl;
                    Peer* newPeer = new Peer { peerIp.str(), peerPort };
                    peers.push_back(newPeer);
                }
            }
    } 

    return peers;
}


void TrackerRequest::ConnectPeer(){
    std::cout <<"ConnectPeer: into connect peer with size "<< peers.size() <<std::endl;
    int x = 0;
    for(auto *peer: peers){
        std::cout << peer->ip<<":" << peer->port <<std::endl;
        if(x){
            x--;
            continue;
        }
        std::cout<<"ConnectPeer: with " << peer->ip<<" " << peer->port <<std::endl;
        int sock = Connect(peer->ip, peer->port);
        TorrentFileParser r_fileparser(*filePath);
        std::cout << "create peerConnection downloadPath" << downloadPath <<std::endl;
        PeerConnection  peerConnection(r_fileparser, downloadPath);
        peerConnection.peer = peer;
        peerConnection.infohash= infohash;
        std::cout <<"ConnectPeer: info hash at connectPeer:"<<peerConnection.infohash<<std::endl;
        peerConnection.peerId = get_PeerID();
        std::cout << "ConnectPeer: after peer id \n";
        peerConnection.PerformHandshake();
            
        std::cout <<"ConnectPeer: socket is "<<sock<<
        "with peer id" << peerConnection.peerId <<std::endl;
    }  
}

