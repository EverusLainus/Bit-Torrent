#include <vector>
#include <string>
/**
 * @brief A single Piece is split into many size of 
 * standard size 2^14. 
 * 
 */
class Block{
    public:
    int piece;
    int offset;
    int length;
    std::string data;

};

/**
 * @brief Data is split into maky pieces. 
 * size of pieces is given in meta-data.
 */

class Piece{
    public:
    int Piece_index;
    std::vector<Block*> blocks;



};