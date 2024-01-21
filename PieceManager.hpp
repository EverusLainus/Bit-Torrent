#include "loguru.hpp"
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include "Piece.hpp"
#define BLOCK_SIZE 16384
struct PendingRequest
{
    Block* block;
    time_t timestamp;
};

class PieceManager{
    public:
    const TorrentFileParser& torrentFileParser;
    int fileLength;
    int pieceLength;
    int blockCount;
    std::ofstream downloadedFile;
    std::string& downloadPath;

    int maximumConnections =5;
    PieceManager(const TorrentFileParser& parser, std::string& downloadPath);
    
    void addPeer(std::string peerId, std::string bitFieldMessage);
    std::vector <Piece *> torrentPieces;
    std::vector <Piece *> missingPieces;
    std::map<std::string, std::string> peers;
    int totalPieces=0;
    std::vector< Piece *> havePieces;
    std::vector <Piece *> ongoingPieces;
    int Complete();
    std::vector<Piece*> initiatePieces();
    std::vector<PendingRequest*> pendingRequests;
    void UpdatePeer(std::string peerId, int index);
    Block* nextRequest(std::string peerId);
    Block* nextOngoing(std::string peerId);
    void writeToDisc(Piece* piece);

    Piece* getNextPieceInSequence(std::string peerId);
    Piece* rarestFirst(std::string peerId);

    void BlockReceived(std::string peerId, int index, int start, std::string data);
};