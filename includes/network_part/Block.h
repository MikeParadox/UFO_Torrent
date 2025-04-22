/**
 * @file Block.h
 * @brief Defines Block structure for piece data transfer.
 * @details Represents a part of a piece that is requested and transferred between peers.
 */
#ifndef BLOCK_H
#define BLOCK_H

#include <string>

 /**
  * @enum BlockStatus
  * @brief Status of a block in the download process.
  */
enum BlockStatus
{
    missing = 0,    /**< Block is missing (not downloaded). */
    pending = 1,    /**< Block is pending (requested but not received). */
    retrieved = 2   /**< Block is retrieved (downloaded and verified). */
};

/**
 * @struct Block
 * @brief Represents a block of data in a piece.
 * @details A Block, by convention, usually has the size of 2^14 bytes,
 *          except for the last Block in a piece.
 */
struct Block
{
    int piece;      /**< Index of the piece this block belongs to. */
    int offset;     /**< Offset within the piece (in bytes). */
    int length;     /**< Length of the block (in bytes). */
    BlockStatus status; /**< Current status of the block. */
    std::string data;   /**< Block data content. */
};

#endif
