#include "TorrentFileParser.h"
#include "Decoder.h"
#include "sha1.h"
#include <bencoding.h>
#include <cassert>
#include <fstream>
#include <sha1.h>
#include <stdexcept>

constexpr int hash_len{20};

/**
 * Constructor of the class TorrentFileParser. Takes in
 * a string that represents the path of the torrent file.
 * parses its content and stores the root of the dictionary
 * to a instance variable named 'root'.
 * @param filePath: path of the torrent file.
 */
TorrentFileParser::TorrentFileParser(const std::string& filePath)
{
    std::ifstream fileStream(filePath, std::ifstream::binary);
    std::shared_ptr<bencoding::BItem> decodedTorrentFile =
        bencoding::decode(fileStream);
    std::shared_ptr<bencoding::BDictionary> rootDict =
        std::dynamic_pointer_cast<bencoding::BDictionary>(decodedTorrentFile);
    root = rootDict;
    //    std::string prettyRepr = bencoding::getPrettyRepr(decodedTorrentFile);
    //    std::cout << prettyRepr << std::endl;
}

/**
 * Retrieves the BItem which has the given key in the decoded Torrent file.
 * Traverses through all the key-value pairs in the parsed dictionary, including
 * sub-dictionaries (i.e. dictionaries which are values).
 */
std::shared_ptr<bencoding::BItem> TorrentFileParser::get(std::string key) const
{
    std::shared_ptr<bencoding::BItem> value = root->getValue(key);
    return value;
}

/**
 * Returns the info hash of the Torrent file as a string.
 * The sha1 function comes from
 */
std::string TorrentFileParser::getInfoHash() const
{
    std::shared_ptr<bencoding::BItem> infoDictionary = get("info");
    std::string infoString = bencoding::encode(infoDictionary);
    std::string sha1Hash = sha1(infoString);
    return sha1Hash;
}

/**
 * Splits the string representation of the value of 'pieces' into
 * a vector of strings.
 */
std::vector<std::string> TorrentFileParser::splitPieceHashes() const
{
    std::shared_ptr<bencoding::BItem> piecesValue = get("pieces");
    if (!piecesValue)
        throw std::runtime_error(
            "Torrent file is malformed. [File does not contain key 'pieces']");
    std::string pieces =
        std::dynamic_pointer_cast<bencoding::BString>(piecesValue)->value();

    std::vector<std::string> pieceHashes;

    assert(pieces.size() % hash_len == 0);
    int piecesCount = (int)pieces.size() / hash_len;
    pieceHashes.reserve(piecesCount);
    for (int i = 0; i < piecesCount; i++)
        pieceHashes.push_back(pieces.substr(i * hash_len, hash_len));
    return pieceHashes;
}

/**
 * Retrieves the total size of the file to be downloaded.
 */
long TorrentFileParser::getFileSize() const
{
    std::shared_ptr<bencoding::BItem> fileSizeItem = get("length");
    if (!fileSizeItem)
        throw std::runtime_error(
            "Torrent file is malformed. [File does not contain key 'length']");
    long fileSize =
        std::dynamic_pointer_cast<bencoding::BInteger>(fileSizeItem)->value();
    return fileSize;
}

/**
 * Retrieves the length of a single piece as per the Torrent file.
 */
long TorrentFileParser::getPieceLength() const
{
    std::shared_ptr<bencoding::BItem> pieceLengthItem = get("piece length");
    if (!pieceLengthItem)
        throw std::runtime_error("Torrent file is malformed. [File does not "
                                 "contain key 'piece length']");
    long pieceLength =
        std::dynamic_pointer_cast<bencoding::BInteger>(pieceLengthItem)
            ->value();
    return pieceLength;
}

/**
 * Retrieves the file name of the file to download. Assuming there is
 * only one downloadable file.
 */
std::string TorrentFileParser::getFileName() const
{
    std::shared_ptr<bencoding::BItem> filenameItem = get("name");
    if (!filenameItem)
        throw std::runtime_error(
            "Torrent file is malformed. [File does not contain key 'name']");
    std::string filename =
        std::dynamic_pointer_cast<bencoding::BString>(filenameItem)->value();
    return filename;
}

/**
 * Retrieves the announce URL from the Torrent file.
 */
std::string TorrentFileParser::getAnnounce() const
{
    std::shared_ptr<bencoding::BItem> announceItem = get("announce");
    if (!announceItem)
        throw std::runtime_error("Torrent file is malformed. [File does not "
                                 "contain key 'announce']");
    std::string announce =
        std::dynamic_pointer_cast<bencoding::BString>(announceItem)->value();
    return announce;
}

