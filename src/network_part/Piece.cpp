#include <algorithm>
#include <cassert>
#include <iostream>
#include <sha1.h>
#include <sstream>
#include <string>
#include <utility>

#include "Piece.h"
#include "utils.h"

Piece::Piece(int index, const std::vector<Block*>& blocks,
             std::string hash_value)
    : _index(index), _hash_value(std::move(hash_value)),
      _blocks{std::move(blocks)}
{
}

Piece::~Piece()
{
    for (Block* block : _blocks) delete block;
}

/**
 * Resets the status of all Blocks in this Piece to Missing.
 */
void Piece::reset()
{
    for (Block* block : _blocks) block->status = missing;
}

/**
 * Finds and returns the next Block to be requested
 * (i.e the first Block that has the status Missing).
 * Changes that Block's status to Pending before returning.
 * If all Blocks are
 */
Block* Piece::next_request()
{
    for (Block* block : _blocks)
    {
        if (block->status == missing)
        {
            block->status = pending;
            return block;
        }
    }
    return nullptr;
}

/**
 * Updates the Block information by setting the status
 * of the Block specified by 'offset' to Retrieved.
 * @param offset: the offset of the Block within  the Piece.
 * @param data: the data contained in the Block.
 */
void Piece::block_received(int offset, const std::string& data)
{
    for (Block* block : _blocks)
    {
        if (block->offset == offset)
        {
            block->status = retrieved;
            block->data = data;
            return;
        }
    }
    throw std::runtime_error("Trying to complete a non-existing block " +
                             std::to_string(offset) + " in piece " +
                             std::to_string(_index));
}

/**
 * Checks if all Blocks within the Piece has been retrieved.
 * Note that this function only checks if the data in the Blocks
 * has been received, it does not calculate the hash, and thus,
 * disregards the correctness of the data.
 */
bool Piece::is_complete()
{
    return std::all_of(_blocks.begin(), _blocks.end(), [](const Block* block)
                       { return block->status == retrieved; });
}

/**
 * Checks if the SHA1 hash for all the retrieved Block data matches
 * the Piece hash from the Torrent meta-info.
 */
bool Piece::is_hash_matching()
{
    std::string pieceHash = hexDecode(sha1(data()));
    return pieceHash == _hash_value;
}

/**
 * Concatenates the data in each Block, and returns it
 * as a whole. Note that for this to succeed, it must be
 * ensured that this Piece is complete.
 * @return the data contained in all the Blocks concatenated
 * as a string;
 */
std::string Piece::data()
{
    assert(is_complete());
    std::stringstream res;
    for (const Block* block : _blocks) res << block->data;
    return res.str();
}





