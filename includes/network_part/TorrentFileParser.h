/**
 * @file TorrentFileParser.h
 * @brief Parses .torrent files.
 * @details Extracts metadata from bencoded torrent files.
 */
#ifndef TORRENTFILEPARSER_H
#define TORRENTFILEPARSER_H

#include "BDictionary.h"
#include "BItem.h"
#include <memory>
#include <string>
#include <vector>

using byte = unsigned char;


/**
* @class TorrentFileParser
* @brief Parses bencoded torrent files.
*/
class TorrentFileParser
{
private:
    std::shared_ptr<bencoding::BDictionary> root; /**< Parsed torrent data. */

public:
    /**
         * @brief Constructs parser and loads file.
         * @param filePath Path to .torrent file.
         */
    explicit TorrentFileParser(const std::string& filePath);

    /**
     * @brief Gets total file size.
     * @return Size in bytes.
     */
    long getFileSize() const;

    /**
     * @brief Gets piece length.
     * @return Piece length in bytes.
     */
    long getPieceLength() const;

    /**
     * @brief Gets output filename.
     * @return Filename string.
     */
    std::string getFileName() const;

    /**
     * @brief Gets tracker URL.
     * @return Announce URL.
     */
    std::string getAnnounce() const;

    /**
     * @brief Gets value by key from torrent.
     * @param key Key to lookup.
     * @return Shared pointer to BItem.
     */
    std::shared_ptr<bencoding::BItem> get(std::string key) const;

    /**
     * @brief Calculates info hash.
     * @return SHA-1 hash string.
     */
    std::string getInfoHash() const;

    /**
     * @brief Splits piece hashes.
     * @return Vector of piece hash strings.
     */
    std::vector<std::string> splitPieceHashes() const;
};

#endif // TORRENTFILEPARSER_H