#include "Piece.hpp"
/**
 * @brief check if the block-offset is present in piece
 * update status to be downloaded if found
 * @param offset block offset in the piece
 * @param data data in the block
 */
void Piece::BlockReceived(int offset, std::string data){
    Block* now_block = nullptr;
    for(Block * block : blocks){
        if(block->offset == offset){
            now_block = block;
            block->status = downloaded;
            block->data = data;
            LOG_F(INFO, " Piece:%d, Block:%d received", block->piece, block->offset);
        }
        
    }
    if(now_block == nullptr){
        std::cout << "error: bock is not found in the piece " << std::endl;
    }
}

/**
 * @brief check if every blocks in the piece is downloaded
 * @return true 
 * @return false 
 */
/*
bool Piece::Complete(){
    std::cout << "Complete: ib\n";
    std::cout << "Complete: "<< blocks.size()<<std::endl;
    for(Block* block : blocks){
        std::cout << "Complete: to in\n";
        if(block->status != downloaded){
            return 0;
        }
        std::cout << "Complete: else\n";
    }
    LOG_F(INFO, "Piece %d received SUCCESS", Piece_index);
    return 1;
}
*/

Block* Piece::nextRequest(){
    std:: cout << "hi from nextResqust\n";
    //LOG_F(INFO, "nextRequest: block size %d", blocks.size());
    //LOG_F(INFO, "nextRequest: piece %d", blocks.size());
    for(Block* block : blocks){
        LOG_F(INFO, "nextRequest: for %d", block->offset);
        if(block->status == missing){
            LOG_F(INFO, "nextRequest: missing");
            block->status = pending;
            LOG_F(INFO, " block with index %d of piece %d is upgraded \
            into ppending state", block->offset, block->piece );
            std::cout << " block "<< block->offset << " fo piece "<< block->piece<<
            "is upgraded to pending state" << std::endl;
            return block;
        }
    }
    return nullptr;
}

bool Piece::Complete()
{
    std::cout << "Complete: "<< blocks.size()<<std::endl;
    return std::all_of(blocks.begin(), blocks.end(),[](Block* block)
       {
           return block->status == downloaded;
       }
    );
}



std::string Piece::Data(){
    if(Complete()){
        std::stringstream data;
        for(Block* block : blocks){
            data << block->data;
        }
        std::cout << "length of data " << data.str().size() <<std::endl;
        return data.str();
    }
    return nullptr;
}

std::string hexDecode(const std::string& value);

bool Piece::hashMatch(){

    if(Data().empty()){
        std::cout << "data  null \n";
    }
    std::string decodedHash = hexDecode(sha1(Data()));
    if(decodedHash == hash){
        return 1;
    }
    return 0;
}

