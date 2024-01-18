#include "loguru.hpp"
#include <string>
#include <map>
#include "Piece.hpp"

struct PendingRequest
{
    Block* block;
    time_t timestamp;
};

class PieceManager{
    public:
    const TorrentFileParser& torrentFileParser;
    int pieceLength;

    PieceManager(const TorrentFileParser& parser): torrentFileParser(parser),
    pieceLength(torrentFileParser.getPieceSize()){}

    std::map<std::string, std::string> peers;
    int totalPieces=0;
    std::vector< Piece *> havePieces;
    std::vector <Piece *> ongoingPieces;
    int Complete();
    std::vector<Piece*> initiatePieces();
    std::vector<PendingRequest*> pendingRequests;
    void UpdatePeer(std::string peerId, int index);
    void BlockReceived(std::string peerId, int index, int start, std::string data);
};