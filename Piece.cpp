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
            std::cout << "error: bock wrt piece is found with offset " << block->offset << std::endl;
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

bool Piece::Complete()
{
    std::cout << "Complete: "<< blocks.size()<<std::endl;
    return std::all_of(blocks.begin(), blocks.end(),[](Block* block)
       {
           return block->status == downloaded;
       }
    );
}

