#include "createHash.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/hex.hpp>

unsigned long long Hash::choosePieceLength(unsigned long long fileSize)
{
    if (fileSize < 512LL * 1024 * 1024) {
        return 16 * 1024;
    }
    else if (fileSize < 1024LL * 1024 * 1024) { 
        return 1024 * 1024;
    }
    else if (fileSize < 2ULL * 1024 * 1024 * 1024) {
        return 4LL * 1024 * 1024; 
    }
    else if (fileSize < 10LL * 1024 * 1024 * 1024) { 
        return 8LL * 1024 * 1024; 
    }
    else if (fileSize < 50LL * 1024 * 1024 * 1024) { 
        return 16LL * 1024 * 1024;
    }
    else { 
        return 32LL * 1024 * 1024;
    }
}

std::string Hash::createHash(const std::vector<std::string>& filePaths, size_t chunk_size) {
    std::string pieces;
    std::vector<char> buffer(chunk_size);
    size_t bufferRead = 0;

    for (const auto& filePath : filePaths) {
        std::ifstream file(filePath, std::ios::binary);

        if (!file)
            throw std::runtime_error("Не удалось открыть файл: " + filePath);


        while (file) {
            file.read(buffer.data() + bufferRead, chunk_size - bufferRead);

            size_t bytesRead = file.gcount();

            bufferRead += bytesRead;

            if (bufferRead == chunk_size || (file.eof() && filePath == filePaths.back())) {
                boost::uuids::detail::sha1 sha;
                sha.process_bytes(buffer.data(), bytesRead); 

                boost::uuids::detail::sha1::digest_type digest;
                sha.get_digest(digest);  

                pieces.append(reinterpret_cast<char*>(digest), 20);
                bufferRead = 0;
            }
        }
    }

    return pieces;
}