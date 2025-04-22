/**
 * @file createHash.h
 * @brief Hash generation utilities for torrent files.
 * @details Provides functions for calculating piece hashes and choosing piece sizes.
 */
#ifndef CREATEHASH_H
#define CREATEHASH_H

#include "./fileUtils.h"
#include <vector>
#include <boost/uuid/detail/sha1.hpp>
#include <string>
#include <stdexcept>

using std::vector;

namespace Hash {

    /**
     * @brief Calculates optimal piece length for torrent.
     * @param fileSize Total size of the file(s) in bytes.
     * @return Recommended piece length in bytes.
     */
    unsigned long long choosePieceLength(unsigned long long fileSize);

    /**
     * @brief Creates SHA-1 hash for torrent pieces.
     * @param filePaths List of files to hash.
     * @param chunk_size Piece size in bytes.
     * @return Concatenated piece hashes as binary string.
     */
    std::string createHash(const std::vector<string>& filePaths, size_t chunk_size);
}

#endif