#include "BitTorrentMessage.hpp"
#include <iostream>
#include <string>
#include <sstream>

BitTorrentMessage::BitTorrentMessage(uint8_t id, const std::string& payload):
        message_id(id), bitfield(payload) {
            message_length = payload.size()+1;
             printf("message id is %d", message_id);
        }

std::string BitTorrentMessage::toString() {
    std::stringstream buffer;
    char* messageLengthAddr = (char*) &message_length;
    std::string messageLengthStr;
    // Bytes are pushed in reverse order, assuming the data
    // is stored in little-endian order locally.
    for (int i = 0; i < 4; i++)
        messageLengthStr.push_back((char) messageLengthAddr[3 - i]);
    buffer << messageLengthStr;
    buffer << (char) message_id;
    buffer << bitfield;
    return buffer.str();
}

std::string BitTorrentMessage::getPayload(){
    return bitfield;
}