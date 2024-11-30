#ifndef CREATEHASH_H
#define CREATEHASH_H

#include "./fileUtils.h"
#include <vector>
#include <boost/uuid/detail/sha1.hpp>
#include <string>
#include <stdexcept>

using std::vector;

namespace Hash {

    unsigned long long choosePieceLength(unsigned long long fileSize);

    std::string createHash(const std::vector<string>& filePaths, size_t chunk_size);
}

#endif