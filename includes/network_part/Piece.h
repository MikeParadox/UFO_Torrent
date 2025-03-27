#ifndef PIECE_H
#define PIECE_H

#include "Block.h"
#include <vector>

/**
 * A class representation of a piece of the Torrent content.
 * Each piece except the final one has a length equal to the
 * value specified by the 'piece length' attribute in the
 * Torrent file.
 * A piece is what is defined in the torrent meta-data. However,
 * when sharing data between peers a smaller unit is used - this
 * smaller piece is refereed to as `Block` by the unofficial
 * specification.
 * The implementation is based on the Python code from the
 * following repository:
 * https://github.com/eliasson/pieces/
 */
class Piece
{
private:
    const std::string _hash_value;
    const int _index;
    std::vector<Block*> _blocks;

public:
    explicit Piece(int index, const std::vector<Block*>& blocks,
                   std::string hash_value);
    ~Piece();
    void reset();
    std::string data();
    Block* next_request();
    void block_received(int offset, const std::string& data);
    bool is_complete();
    int index() const { return _index; }
    bool is_hash_matching();
};

#endif // PIECE_H
