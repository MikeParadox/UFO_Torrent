[33mcommit d2dfbcfa0a8be60f8593450445f350ba605ecd34[m[33m ([m[1;36mHEAD[m[33m -> [m[1;32mnetwork_part[m[33m, [m[1;31morigin/network_part[m[33m)[m
Author: Mikhail <hudih@mail.ru>
Date:   Thu Mar 27 21:43:26 2025 +0300

    Added parser for a client

[1mdiff --git a/includes/network_part/TorrentFileParser.h b/includes/network_part/TorrentFileParser.h[m
[1mnew file mode 100644[m
[1mindex 0000000..5d6e565[m
[1m--- /dev/null[m
[1m+++ b/includes/network_part/TorrentFileParser.h[m
[36m@@ -0,0 +1,35 @@[m
[32m+[m[32m#ifndef TORRENTFILEPARSER_H[m
[32m+[m[32m#define TORRENTFILEPARSER_H[m
[32m+[m[32m#include <memory>[m
[32m+[m[32m#include <string>[m
[32m+[m[32m#include <vector>[m
[32m+[m
[32m+[m[32musing byte = unsigned char;[m
[32m+[m[32m/**[m
[32m+[m[32m * A class that parses a given Torrent file by using the bencoding library in[m
[32m+[m[32m * following repo: https://github.com/s3rvac/cpp-bencoding. The result returned[m
[32m+[m[32m * by bencoding decoder is a pointer to a custom BItem object, which can be a[m
[32m+[m[32m * dictionary, a list, an integer, or a string. To retrieve the value of a[m
[32m+[m[32m * specific key in the top level dictionary, an instance function named[m
[32m+[m[32m * getValue() was added to the BDictionary class. It recursively checks the keys[m
[32m+[m[32m * of all dictionaries from the top level and returns the value of key, if it[m
[32m+[m[32m * exists.[m
[32m+[m[32m */[m
[32m+[m[32mclass TorrentFileParser[m
[32m+[m[32m{[m
[32m+[m[32mprivate:[m
[32m+[m[32m    std::shared_ptr<bencoding::BDictionary> root;[m
[32m+[m
[32m+[m[32mpublic:[m
[32m+[m[32m    explicit TorrentFileParser(const std::string& filePath);[m
[32m+[m[32m    long getFileSize() const;[m
[32m+[m[32m    long getPieceLength() const;[m
[32m+[m[32m    std::string getFileName() const;[m
[32m+[m[32m    std::string getAnnounce() const;[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> get(std::string key) const;[m
[32m+[m[32m    std::string getInfoHash() const;[m
[32m+[m[32m    std::vector<std::string> splitPieceHashes() const;[m
[32m+[m[32m};[m
[32m+[m
[32m+[m
[32m+[m[32m#endif // BITTORRENTCLIENT_TORRENTFILEPARSER_H[m
\ No newline at end of file[m
[1mdiff --git a/src/network_part/TorrentFileParser.cpp b/src/network_part/TorrentFileParser.cpp[m
[1mnew file mode 100644[m
[1mindex 0000000..7006f5d[m
[1m--- /dev/null[m
[1m+++ b/src/network_part/TorrentFileParser.cpp[m
[36m@@ -0,0 +1,137 @@[m
[32m+[m[32m#include "TorrentFileParser.h"[m
[32m+[m[32m#include "sha1.h"[m
[32m+[m[32m#include <bencode/BItem.h>[m
[32m+[m[32m#include <bencode/Decoder.h>[m
[32m+[m[32m#include <bencode/bencoding.h>[m
[32m+[m[32m#include <cassert>[m
[32m+[m[32m#include <crypto/sha1.h>[m
[32m+[m[32m#include <fstream>[m
[32m+[m[32m#include <iostream>[m
[32m+[m[32m#include <regex>[m
[32m+[m[32m#include <stdexcept>[m
[32m+[m
[32m+[m[32mconstexpr int hash_len{20};[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Constructor of the class TorrentFileParser. Takes in[m
[32m+[m[32m * a string that represents the path of the torrent file.[m
[32m+[m[32m * parses its content and stores the root of the dictionary[m
[32m+[m[32m * to a instance variable named 'root'.[m
[32m+[m[32m * @param filePath: path of the torrent file.[m
[32m+[m[32m */[m
[32m+[m[32mTorrentFileParser::TorrentFileParser(const std::string& filePath)[m
[32m+[m[32m{[m
[32m+[m[32m    std::ifstream fileStream(filePath, std::ifstream::binary);[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> decodedTorrentFile =[m
[32m+[m[32m        bencoding::decode(fileStream);[m
[32m+[m[32m    std::shared_ptr<bencoding::BDictionary> rootDict =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BDictionary>(decodedTorrentFile);[m
[32m+[m[32m    root = rootDict;[m
[32m+[m[32m    //    std::string prettyRepr = bencoding::getPrettyRepr(decodedTorrentFile);[m
[32m+[m[32m    //    std::cout << prettyRepr << std::endl;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Retrieves the BItem which has the given key in the decoded Torrent file.[m
[32m+[m[32m * Traverses through all the key-value pairs in the parsed dictionary, including[m
[32m+[m[32m * sub-dictionaries (i.e. dictionaries which are values).[m
[32m+[m[32m */[m
[32m+[m[32mstd::shared_ptr<bencoding::BItem> TorrentFileParser::get(std::string key) const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> value = root->getValue(key);[m
[32m+[m[32m    return value;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Returns the info hash of the Torrent file as a string.[m
[32m+[m[32m * The sha1 function comes from[m
[32m+[m[32m */[m
[32m+[m[32mstd::string TorrentFileParser::getInfoHash() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> infoDictionary = get("info");[m
[32m+[m[32m    std::string infoString = bencoding::encode(infoDictionary);[m
[32m+[m[32m    std::string sha1Hash = sha1(infoString);[m
[32m+[m[32m    return sha1Hash;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Splits the string representation of the value of 'pieces' into[m
[32m+[m[32m * a vector of strings.[m
[32m+[m[32m */[m
[32m+[m[32mstd::vector<std::string> TorrentFileParser::splitPieceHashes() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> piecesValue = get("pieces");[m
[32m+[m[32m    if (!piecesValue)[m
[32m+[m[32m        throw std::runtime_error([m
[32m+[m[32m            "Torrent file is malformed. [File does not contain key 'pieces']");[m
[32m+[m[32m    std::string pieces =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BString>(piecesValue)->value();[m
[32m+[m
[32m+[m[32m    std::vector<std::string> pieceHashes;[m
[32m+[m
[32m+[m[32m    assert(pieces.size() % HASH_LEN == 0);[m
[32m+[m[32m    int piecesCount = (int)pieces.size() / hash_len;[m
[32m+[m[32m    pieceHashes.reserve(piecesCount);[m
[32m+[m[32m    for (int i = 0; i < piecesCount; i++)[m
[32m+[m[32m        pieceHashes.push_back(pieces.substr(i * hash_len, hash_len));[m
[32m+[m[32m    return pieceHashes;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Retrieves the total size of the file to be downloaded.[m
[32m+[m[32m */[m
[32m+[m[32mlong TorrentFileParser::getFileSize() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> fileSizeItem = get("length");[m
[32m+[m[32m    if (!fileSizeItem)[m
[32m+[m[32m        throw std::runtime_error([m
[32m+[m[32m            "Torrent file is malformed. [File does not contain key 'length']");[m
[32m+[m[32m    long fileSize =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BInteger>(fileSizeItem)->value();[m
[32m+[m[32m    return fileSize;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Retrieves the length of a single piece as per the Torrent file.[m
[32m+[m[32m */[m
[32m+[m[32mlong TorrentFileParser::getPieceLength() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> pieceLengthItem = get("piece length");[m
[32m+[m[32m    if (!pieceLengthItem)[m
[32m+[m[32m        throw std::runtime_error("Torrent file is malformed. [File does not "[m
[32m+[m[32m                                 "contain key 'piece length']");[m
[32m+[m[32m    long pieceLength =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BInteger>(pieceLengthItem)[m
[32m+[m[32m            ->value();[m
[32m+[m[32m    return pieceLength;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Retrieves the file name of the file to download. Assuming there is[m
[32m+[m[32m * only one downloadable file.[m
[32m+[m[32m */[m
[32m+[m[32mstd::string TorrentFileParser::getFileName() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> filenameItem = get("name");[m
[32m+[m[32m    if (!filenameItem)[m
[32m+[m[32m        throw std::runtime_error([m
[32m+[m[32m            "Torrent file is malformed. [File does not contain key 'name']");[m
[32m+[m[32m    std::string filename =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BString>(filenameItem)->value();[m
[32m+[m[32m    return filename;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m[32m/**[m
[32m+[m[32m * Retrieves the announce URL from the Torrent file.[m
[32m+[m[32m */[m
[32m+[m[32mstd::string TorrentFileParser::getAnnounce() const[m
[32m+[m[32m{[m
[32m+[m[32m    std::shared_ptr<bencoding::BItem> announceItem = get("announce");[m
[32m+[m[32m    if (!announceItem)[m
[32m+[m[32m        throw std::runtime_error("Torrent file is malformed. [File does not "[m
[32m+[m[32m                                 "contain key 'announce']");[m
[32m+[m[32m    std::string announce =[m
[32m+[m[32m        std::dynamic_pointer_cast<bencoding::BString>(announceItem)->value();[m
[32m+[m[32m    return announce;[m
[32m+[m[32m}[m
