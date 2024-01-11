#include <inttypes.h>
#include <string>

enum MessageId
{
    keepAlive = -1,
    choke = 0,
    unchoke = 1,
    interested = 2,
    notInterested = 3,
    have = 4,
    bitField = 5,
    request = 6,
    piece = 7,
    cancel = 8,
    port = 9
};

class BitTorrentMessage{
    public:
    uint32_t message_length;
    uint8_t message_id;
    std::string bitfield;
    
    explicit BitTorrentMessage(uint8_t message_id, const std::string& payload = "");
    BitTorrentMessage(){};
};