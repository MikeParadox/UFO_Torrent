/**
 * @file torrentFile.h
 * @brief Torrent file metadata structures and utilities.
 * @details Defines TorrentFile structure and related operations.
 */
#ifndef TORRENTFILE_H
#define TORRENTFILE_H

#include <boost/variant.hpp>
#include <ctime>
#include <optional>
#include <stdexcept>
#include "valueTypes.h"
#include "createHash.h"
#include "fileUtils.h"

using std::string;
using std::vector;
using std::optional;

using namespace bencode;

namespace Torrent {

    /**
     * @struct TorrentFileInfo
     * @brief Metadata about a single file in torrent.
     */
    struct TorrentFileInfo {
        unsigned long long length;  /**< File size in bytes. */
        vector<string> path;        /**< File path components. */
    };

    /**
     * @struct TorrentFile
     * @brief Complete torrent file metadata.
     */
    struct TorrentFile {
        string announce;                     /**< Primary tracker URL. */
        vector<vector<string>> announceList; /**< Alternate tracker URLs. */
        optional<string> createdBy;          /**< Client that created torrent. */
        optional<time_t> creationDate;       /**< Creation timestamp. */

        /**
         * @struct Info
         * @brief Info dictionary from torrent file.
         */
        struct Info {
            vector<TorrentFileInfo> files;  /**< List of files. */
            string name;                    /**< Torrent name. */
            unsigned long long pieceLength; /**< Piece size in bytes. */
            string pieces;                  /**< Concatenated piece hashes. */
        } info;
    };

    /**
     * @struct ValueToField
     * @brief Visitor for converting Value to TorrentFile fields.
     */
    struct ValueToField : public boost::static_visitor<> {
        TorrentFile& torrent; /**< Target TorrentFile. */
        TorrentFile::Info& info; /**< Reference to info dict. */

        /**
         * @brief Constructs ValueToField.
         * @param t Target TorrentFile.
         */
        explicit ValueToField(TorrentFile& t)
            : torrent(t), info(t.info) {
        }

        void operator()(int value) const;
        void operator()(const string& value) const;
        void operator()(const ValueVector& values) const;
        void operator()(const ValueDictionary& dict) const;

        mutable string currentKey; /**< Current dictionary key. */
        mutable TorrentFileInfo currentFile; /**< Current file being processed. */
    };

    /**
     * @brief Computes info hash for torrent.
     * @param torrent Torrent file metadata.
     * @return SHA-1 hash as binary string.
     */
    string computeInfoHash(const TorrentFile& torrent);

    /**
     * @brief Parses Value into TorrentFile.
     * @param data Decoded bencode Value.
     * @return Populated TorrentFile.
     */
    TorrentFile parseTorrentFile(const Value& data);

    /**
     * @brief Converts TorrentFile to bencode Value.
     * @param torrent Torrent file metadata.
     * @return Encoded Value.
     */
    Value toValue(const TorrentFile& torrent);

    /**
     * @brief Creates new torrent file.
     * @param announce Tracker URL.
     * @param announceList Alternate trackers.
     * @param name Torrent name.
     * @param folderPath Path to content.
     * @param createdBy Optional creator string.
     * @param creationDate Optional creation timestamp.
     * @return New TorrentFile.
     */
    TorrentFile createTorrentFile(const string& announce, const vector<vector<string>>& announceList,
        const string& name, const string& folderPath,
        optional<string> createdBy = std::nullopt,
        optional<time_t> creationDate = std::nullopt);
};

#endif