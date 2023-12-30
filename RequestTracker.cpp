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
std::string get_PeerID(){ 

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
std::vector <peers *> get_peers(){
    TrackerRequest trackerRequest;
    std::string anounceurl = trackerRequest.get_announce();
    std::string infohash = trackerRequest.infohash;
    std::string peerid = get_PeerID();
    int port = trackerRequest.port;

    //construct the tracker request URL
    std::string trackerUrl = anounceurl + "?info_hash="+ infohash+
    "&peer_id="+peerid 
    +"&port="+std::to_string(port)
    +"&uploaded="+std::to_string(trackerRequest.uploaded)
    +"&downloaded=" +std::to_string(trackerRequest.downloaded)
    +"&left=" + std::to_string(trackerRequest.left)
    +"&event=" + std::to_string(trackerRequest.event);

    //send the tracker request
    auto response = cpr::Get(cpr::Url{trackerUrl});
    

    //check if the request was successful
    if(response.status_code == 200){
        std::cout <<"Tracker response: "<<response.text <<std::endl;
        std::shared_ptr<bencoding::BItem> decodedResponse = bencoding::Decode(response);
            if (auto dictionary = std::dynamic_pointer_cast<bencoding::BDictionary>(decodedResponse)) {
            // Access and print the parsed information
            for (const auto& entry : dictionary->getMap()) {
                std::cout << entry.first << ": " << entry.second->toString() << std::endl;
            }
    }else{
        std::cerr <<"Tracker request failed with status code: "<<response.status_code<<std::endl;
    }
}

void TrackerResponse(){

}

