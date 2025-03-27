#ifndef TORRENTFILEPARSER_H
#define TORRENTFILEPARSER_H

#include "BDictionary.h"
#include "BItem.h"
#include <memory>
#include <string>
#include <vector>

using byte = unsigned char;
/**
 * A class that parses a given Torrent file. The result returned
 * by bencoding decoder is a pointer to a custom BItem object, which can be a
 * dictionary, a list, an integer, or a string. To retrieve the value of a
 * specific key in the top level dictionary, an instance function named
 * getValue() was added to the BDictionary class. It recursively checks the keys
 * of all dictionaries from the top level and returns the value of key, if it
 * exists.
 */
class TorrentFileParser
{
private:
    std::shared_ptr<bencoding::BDictionary> root;

public:
    explicit TorrentFileParser(const std::string& filePath);
    long getFileSize() const;
    long getPieceLength() const;
    std::string getFileName() const;
    std::string getAnnounce() const;
    std::shared_ptr<bencoding::BItem> get(std::string key) const;
    std::string getInfoHash() const;
    std::vector<std::string> splitPieceHashes() const;
};


#endif // BITTORRENTCLIENT_TORRENTFILEPARSER_H