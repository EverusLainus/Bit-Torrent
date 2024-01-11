#include "BitTorrentMessage.hpp"

BitTorrentMessage::BitTorrentMessage(uint8_t id, const std::string& payload):
        message_id(message_id), bitfield(payload) {}