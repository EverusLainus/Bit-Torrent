#include <string>
#include "PieceManager.hpp"


/**
 * Sets the given index of the Bitfield to 1.
 */
void setPiece(std::string& bitField, int index)
{
    int byteIndex = floor(index / 8);
    int offset = index % 8;
    bitField[byteIndex] |= (1 << (7 - offset));
    printf("bitfiels updates : %s", bitField.c_str());
    LOG_F(INFO, " bitfield of index %d is %s", index, bitField.c_str());
}

std::vector<Piece*> PieceManager::initiatePieces(){
    std::vector<std::string> pieceHashes = torrentFileParser.splitPieceHashes();
    totalPieces = pieceHashes.size();


}

/**
 * @brief checks if every pieces are received
 * 
 * @return int 
 */
int PieceManager::Complete(){
    std::cout << "Complete: in \n";
    std::cout << totalPieces << std::endl;
    if(havePieces.size() == totalPieces){
        return 1;
    }
    std::cout << "Complete: out \n";
    return 0;
}

/**
 * @brief given a peerId update its bit field
 * after a have message
 * @param peerId 
 */
void PieceManager::UpdatePeer(std::string peerId, int index){
    setPiece(peers[peerId], index);
    
}

/**
 * @brief after getting piece message
 * 
 * @param peerId 
 * @param index 
 * @param start 
 * @param data 
 */
void PieceManager::BlockReceived(std::string peerId, int pieceIndex, int blockOffset, std::string data){
    LOG_F(INFO, "Received block %d for piece %d from peer %s", blockOffset, pieceIndex, peerId.c_str());
    std::cout << "blockReceived: \n";
    PendingRequest* request_to_remove = nullptr;
    for(PendingRequest* pending_request : pendingRequests){
        if(pending_request->block->offset == pieceIndex && 
        pending_request->block->piece == blockOffset){
            request_to_remove = pending_request;
            break;
        }
    }
    std::cout << "blockReceived: for\n";
    auto Ptr_to_newEnd = std::remove(pendingRequests.begin(), pendingRequests.end(), request_to_remove);
    pendingRequests.erase(Ptr_to_newEnd, pendingRequests.end());

    Piece* now_piece = nullptr;
    //from the ongoing pieces, find the piece to which this block belongs
    for(Piece* piece : ongoingPieces){
        if(piece->Piece_index == pieceIndex){ 
            now_piece = piece;
            piece->BlockReceived(blockOffset, data);
        }       
    }
    std::cout << "blockReceived: for 2\n";
    //check if all blocks of the piece are received
    if(now_piece->Complete()){ }

}