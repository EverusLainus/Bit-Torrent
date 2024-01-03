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

//get announce URL
std::string TrackerRequest::get_PeerID(){ 
    std::cout << "into peers ID"<<std::endl;
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(20);
    for(size_t i=0; i<20 ; ++i){
        result.push_back(charset[dist(rng)]);
    }
    std::cout <<" peerId : "<< result <<std::endl;
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
    std::cout <<"urlencoded peerId : "<< encoded.str()<<std::endl;

    // Convert the output stream to a string and return
    return encoded.str();
}

//assumes someone has filled the tracker Request class
void TrackerRequest::get_peers(std::string& file_path){
    std::cout << "into get_peers "<<std::endl;
    TorrentFileParser torrentFileParser(file_path);
    std::cout << "get peers: after torrent file parser "<<std::endl;
    std::string anounceurl = torrentFileParser.getAnnounce();
    std::cout << "get peers: after announce "<<std::endl;
    std::string info = torrentFileParser.getInfoHash();
    std::cout << "get peers: after hash"<<std::endl;
    std::string peerid = get_PeerID();
    std::cout << "get peers: after peer id "<<std::endl;
    int port = 2300;

    //construct the tracker request URL
    std::string trackerUrl = anounceurl + "?info_hash="+ info+
    "&peer_id="+peerid 
    +"&port="+std::to_string(port)
    +"&uploaded="+std::to_string(0)
    +"&downloaded=" +std::to_string(1)
    +"&left=" + std::to_string(1)
    +"&compact=" + std::to_string(1);

    //send the tracker request
    auto response = cpr::Get(cpr::Url{trackerUrl});
    

    //check if the request was successful
    if(response.status_code == 200){
        std::cout <<"Tracker response: "<<response.text <<std::endl;
        /*
        std::shared_ptr<bencoding::BItem> decodedResponse = bencoding::Decode(response);
            if (auto dictionary = std::dynamic_pointer_cast<bencoding::BDictionary>(decodedResponse)) {
            // Access and print the parsed information
            for (const auto& entry : dictionary.) {
                std::cout << entry.first << ": " << entry.second->toString() << std::endl;
            }
            */
    }else{
        std::cerr <<"Tracker request failed with status code: "<<response.status_code<<std::endl;
    }
}


