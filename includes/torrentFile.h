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

	// file info
    struct TorrentFileInfo {
        unsigned long long length;                      // file size
        vector<string> path;                            // file path
    };

	// torrent file
    struct TorrentFile {

		string announce;                                // tracker
		vector<vector<string>> announceList;            // alternate trackers
		optional<string> createdBy;                     // owner
		optional<time_t> creationDate;                  // date of creation

        struct Info {
			vector<TorrentFileInfo> files;              // all files
            string name;                                // name 
			unsigned long long pieceLength;             // piece size
            string pieces;                              // hash
        } info;
    };

    struct ValueToField : public boost::static_visitor<> {
        TorrentFile& torrent;
        TorrentFile::Info& info;

        explicit ValueToField(TorrentFile& t)
            : torrent(t), info(t.info) {}

        void operator()(int value) const;

        void operator()(const string& value) const;

        void operator()(const ValueVector& values) const;

        void operator()(const ValueDictionary& dict) const;

        mutable string currentKey;
        mutable TorrentFileInfo currentFile;
    };

    // value to TorrentFile
    TorrentFile parseTorrentFile(const Value& data);

    Value toValue(const TorrentFile& torrent);

    TorrentFile createTorrentFile(const string& announce,  const vector<vector<string>>& announceList, const string& name, const string& folderPath, optional<string> createdBy = std::nullopt, optional<time_t> creationDate = std::nullopt);
};

#endif