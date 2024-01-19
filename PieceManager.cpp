#include <string>
#include "PieceManager.hpp"
#include <filesystem>


PieceManager::PieceManager(const TorrentFileParser& parser, std::string& downloadPath): torrentFileParser(parser),
    pieceLength(torrentFileParser.getPieceSize()), downloadPath(downloadPath){
        missingPieces = initiatePieces();
        LOG_F(INFO, "PieceManager- constructor: downloadPath is %s", downloadPath.c_str());
    }

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

void PieceManager::addPeer(std::string peerId, std::string bitFieldMessage){
    peers[peerId] = bitFieldMessage;

    //print
    std::stringstream info;
    info << "Number of connections: " <<
         std::to_string(peers.size()) << "/" + std::to_string(maximumConnections);
    std::cout << info.str() << std::endl;
    LOG_F(INFO, "%s", info.str().c_str());
}

/**
 * Checks if the bit at the given index of a BitField string is set to
 * 1 or not.
 */
bool hasPiece(const std::string& bitField, int index)
{
    int byteIndex = floor(index / 8);
    int offset = index % 8;
    return (bitField[byteIndex] >> (7 - offset) & 1) != 0;
}

std::vector<Piece*> PieceManager::initiatePieces(){


    //std::string fileName = "example.pdf";
    //std::string downloadPath = "/Users/everus/C-Programming/my_torrent/downloads/" + fileName;

        if (!std::filesystem::exists(downloadPath)) {
            // Handle the case where the file doesn't exist
            LOG_F(INFO, "file doesnt exist already");
                //create a file; truncate it according to file size
            downloadedFile.open(downloadPath, std::ios::binary | std::ios::out | std::ios::in);
            downloadedFile.seekp(torrentFileParser.getFileSize() - 1);
            downloadedFile.write("", 1);
            if (!downloadedFile) {
                std::cerr << "Error truncating the file: " << downloadPath << std::endl;
                LOG_F(INFO, "error turncating");
            }
        }
        

    std::cout << "initiatePieces:  in \n";
    LOG_F(INFO, "Initating Pieces ");
    std::vector<std::string> pieceHashes = torrentFileParser.splitPieceHashes();

    totalPieces = pieceHashes.size();
    fileLength = torrentFileParser.getFileSize();
    LOG_F(INFO, "totalPieces:%d, fileSize:%d", totalPieces, fileLength);
    pieceLength = torrentFileParser.getPieceSize();
    blockCount = ceil(pieceLength/BLOCK_SIZE);
    int LastPieceSize;
    for(int i=0; i < totalPieces; ++i){
        if((i == totalPieces-1)){
            LastPieceSize = fileLength % pieceLength;
            
            blockCount = ((LastPieceSize+ BLOCK_SIZE -1)/BLOCK_SIZE);
            LOG_F(INFO, "last piece size:%d, blockCount: %d, pieceLength: %d", LastPieceSize, blockCount, pieceLength);
        }
        LOG_F(INFO, "block count:%d", blockCount);
        std::vector<Block*> blocks;
        blocks.reserve(blockCount);
        for(int j=0; j < blockCount; ++j){
            Block* new_block = new Block();
            new_block->piece = i;
            new_block->offset = j;
            new_block->status = missing;
            new_block->length = BLOCK_SIZE;
            if((i == totalPieces-1) && (j== blockCount-1)){
                new_block->length = LastPieceSize % BLOCK_SIZE;
                LOG_F(INFO, "last block %d, last piece %d length: %d",j, i, new_block->length);
            }
            blocks.push_back(new_block);
            LOG_F(INFO, "created: piece:%d, offset:%d, length:%d",new_block->piece,new_block->offset, new_block->length );
        }
        Piece* new_piece = new Piece();
        new_piece->blocks = blocks;
        new_piece->Piece_index = i;
        new_piece->hash = pieceHashes[i];
        torrentPieces.emplace_back(new_piece);
        
    }
    std::cout << "initiatePieces:  out \n";
    return torrentPieces;
}

Block* PieceManager::nextRequest(std::string peerId){
    std::cout << "nextRequest: " <<std::endl;
    //if there are no missing pieces return null ptr
    if(missingPieces.size() == 0){
        std::cout << "nextRequest: there are no missing pieces \n";
        return nullptr;
    }

    //find the given peerId
    if(peers.find(peerId) == peers.end()){
        std::cout << "nextRequest: peerId not found \n";
        return nullptr;
    }
    //if there is a long waiting block remove it

    //new block from nogoing block
    Block* block = nextOngoing(peerId);
    Piece * now_piece = nullptr;
    if(!block){
        std::cout << "got bock from next peice\n";
        LOG_F(INFO,"got bock from next peice");
        now_piece = getNextPieceInSequence(peerId);
        std::cout << "now_piece"<<now_piece->Piece_index<<std::endl;
        std::cout << "next getNextPieceInSequence \n";
        if(!now_piece){
            std::cout << "next is null \n";
        }
        block = now_piece->nextRequest();
        std::cout << "next block is get: "<< block->piece << " " << block->offset << std::endl;
    } 
    std::cout << "nextRequest: return from nextRequest " << block->offset <<"of piece "
    << block->piece <<std::endl;
    LOG_F(INFO, "nextReqeust :%d, block:%d, length:%d", block->piece, block->offset, block->length);
    return block;
}

Block* PieceManager::nextOngoing(std::string peerId)
{
    LOG_F(INFO, " size of ongoing pices %d", ongoingPieces.size());
    for (Piece* piece : ongoingPieces)
    {
        if (hasPiece(peers[peerId], piece->Piece_index))
        {
            Block* block = piece->nextRequest();
            if (block)
            {
                auto currentTime = std::time(nullptr);
                auto newPendingRequest = new PendingRequest;
                newPendingRequest->block = block;
                newPendingRequest->timestamp = std::time(nullptr);
                pendingRequests.push_back(newPendingRequest);
                LOG_F(INFO, "nextOngoing :%d, block:%d, length:%d", block->piece, block->offset, block->length);
                return block;
            }
        }
    }
    return nullptr;
}


//alternative for rarest piece first
Piece* PieceManager::getNextPieceInSequence(std::string peerId){

    //for each piece in the missing piece
    Piece* my_piece = nullptr;
    for(int i=0; i< missingPieces.size(); ++i){
        // if ther is such a peerId
        if(peers.find(peerId) != peers.end()){
           // if the peerid has the piece
            if(hasPiece(peers[peerId], missingPieces[i]->Piece_index)){
                my_piece = missingPieces[i];
                std::cout << "has piece of peerId" << peerId << 
                " with index " << my_piece->Piece_index << std::endl;
            }           
            break;        
        }
        std::cout << "such a peer does not exist" << std::endl;
    }
        //remove from the missing pieces
        std::cout << "missing pieces size : "<<missingPieces.size() << std::endl;
        missingPieces.erase( std::remove(missingPieces.begin(), missingPieces.end(), my_piece), missingPieces.end());

        //add to ongoing pieces
        ongoingPieces.emplace_back(my_piece);
        std::cout << "out " << std::endl;
    
    return my_piece;
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
        return 0;
    }
    std::cout << "Complete: out \n";
    return 1;
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
    std::cout <<"no of pending requests " << pendingRequests.size() <<std::endl;
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
    std::cout <<"no of ongoingPieces " << ongoingPieces.size() <<std::endl;
    for(Piece* piece : ongoingPieces){
        if(piece->Piece_index == pieceIndex){ 
            now_piece = piece;
            piece->BlockReceived(blockOffset, data);
        }       
    }
    std::cout << "blockReceived: for 2\n";


    //check if all blocks of the piece are received
    if(now_piece->Complete()){ 
        std::cout << "into if "<< std::endl;
        if(now_piece->hashMatch()){
            LOG_F(INFO, " Piece:%d, Hashed Match", now_piece->Piece_index);
        }
        writeToDisc(now_piece);
        LOG_F(INFO, "written to disc");
    
    
    }

    if(now_piece->Piece_index == 9){
        downloadedFile.close();
    }



}

void PieceManager::writeToDisc(Piece* piece)
{
    std::cout << "writeToDisc: in\n";
    if (!downloadedFile.is_open()) {
        std::cerr << "Error opening the file: " << downloadPath << std::endl;
        LOG_F(INFO, "error opening file %s", downloadPath.c_str());

        if (downloadedFile.bad()) {
            std::cerr << "Stream is bad (unrecoverable error)." << std::endl;
        } else if (downloadedFile.fail()) {
            std::cerr << "Stream operation failed." << std::endl;
        } else if (downloadedFile.eof()) {
            std::cerr << "End of file reached unexpectedly." << std::endl;
        } else {
            std::cerr << "Unknown error." << std::endl;
        }
        return;
    }    
    long position = piece->Piece_index * torrentFileParser.getPieceSize();
    downloadedFile.seekp(position);
    long currentPosition = downloadedFile.tellp();
    if (currentPosition != position) {
        LOG_F(INFO, "Error in file position.");
        std::cerr << "Error in file position." << std::endl;
        return;
    }
    downloadedFile << piece->Data() <<std::endl;
    if (!downloadedFile.is_open() || downloadedFile.fail()) {
        LOG_F(INFO, "Error writing data to the file.");
        std::cerr << "Error writing data to the file." << std::endl;
        return;
    }

    // Close the file
    downloadedFile.close();
}