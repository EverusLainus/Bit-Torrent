//give request to the tracker
#include <sstream> //for ostringstream
#include <string>
#include <iomanip> //for setw()
#include <iostream>
#include <random>

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

tracker_request(){
    std::string announceUrl = TorrentFileParser::getAnnounce();
    std::string info_hash = torrentFileParser::getInfoHash();
    
}

