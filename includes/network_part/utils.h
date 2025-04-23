/**
 * @file utils.h
 * @brief Utility functions.
 * @details Various helper functions for torrent client.
 */
#ifndef UTILS_H
#define UTILS_H

#include <string>

 /**
  * @brief URL-encodes a string.
  * @param value Input string.
  * @return URL-encoded string.
  */
std::string urlEncode(const std::string& value);

/**
 * @brief Decodes hex string.
 * @param value Hex string.
 * @return Decoded binary string.
 */
std::string hexDecode(const std::string& value);


/**
 * @brief Encodes string to hex.
 * @param input Binary string.
 * @return Hex string.
 */
std::string hexEncode(const std::string& input);

/**
 * @brief Checks if piece is available in bitfield.
 * @param bitField Peer's bitfield.
 * @param index Piece index.
 * @return true if piece is available.
 */
bool hasPiece(const std::string& bitField, int index);

/**
 * @brief Sets piece in bitfield.
 * @param bitField Bitfield to modify.
 * @param index Piece index to set.
 */
void setPiece(std::string& bitField, int index);

/**
 * @brief Converts 4 bytes to integer.
 * @param bytes 4-byte string.
 * @return Integer value.
 */
int bytesToInt(std::string bytes);

/**
 * @brief Formats seconds as HH:MM:SS.
 * @param seconds Time in seconds.
 * @return Formatted time string.
 */
std::string formatTime(long seconds);

#endif // UTILS_H
