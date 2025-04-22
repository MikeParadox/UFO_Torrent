/**
 * @file Piece.h
 * @brief Defines Piece class for torrent content.
 * @details Represents a piece of torrent content and its blocks.
 */
#ifndef PIECE_H
#define PIECE_H

#include "Block.h"
#include <vector>

 /**
  * @class Piece
  * @brief Represents a piece of torrent content.
  */
class Piece
{
private:
    const std::string _hash_value;  /**< Expected SHA-1 hash of piece data. */
    const int _index;               /**< Piece index. */
    std::vector<Block*> _blocks;    /**< Blocks comprising this piece. */

public:
    /**
         * @brief Constructs Piece.
         * @param index Piece index.
         * @param blocks Vector of blocks.
         * @param hash_value Expected SHA-1 hash.
         */
    explicit Piece(int index, const std::vector<Block*>& blocks,
        std::string hash_value);
    /**
     * @brief Destructs Piece.
     */
    ~Piece();

    /**
     * @brief Resets piece state.
     */
    void reset();

    /**
     * @brief Gets piece data.
     * @return Concatenated block data.
     */
    std::string data();

    /**
     * @brief Gets next block to request.
     * @return Pointer to next Block, or nullptr if none.
     */
    Block* next_request();

    /**
     * @brief Handles received block.
     * @param offset Block offset.
     * @param data Block data.
     */
    void block_received(int offset, const std::string& data);
    /**
     * @brief Checks if piece is complete.
     * @return true if all blocks are retrieved.
     */
    bool is_complete();
    /**
     * @brief Gets piece index.
     * @return Piece index.
     */
    int index() const { return _index; }
    /**
     * @brief Verifies piece hash.
     * @return true if hash matches.
     */
    bool is_hash_matching();
};

#endif // PIECE_H

