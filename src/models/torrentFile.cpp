#include "../includes/torrentFile.h"
#include "torrentFile.h"
#include <filesystem>

using std::string;
using std::vector;
using std::optional;

namespace fs = std::filesystem;

using namespace bencode;
using namespace Torrent;

void ValueToField::operator()(int value) const {
	if (currentKey == "creation date") {
		torrent.creationDate = value;
	}
	else if (currentKey == "piece length") {
		info.pieceLength = value;
	}
}

void ValueToField::operator()(const string& value) const {
	if (currentKey == "announce") {
		torrent.announce = value;
	}
	else if (currentKey == "created by") {
		torrent.createdBy = value;
	}
	else if (currentKey == "name") {
		info.name = value;
	}
	else if (currentKey == "pieces") {
		info.pieces = value;
	}
}

void ValueToField::operator()(const ValueVector& values) const {
	if (currentKey == "announce-list") {
		vector<string> trackers;
		for (const auto& v : values) {
			if (v.type() == typeid(ValueVector)) {
				vector<string> group;
				for (const auto& nested : boost::get<ValueVector>(v)) {
					group.push_back(boost::get<string>(nested));
				}
				torrent.announceList.push_back(group);
			}
		}
	}
	else if (currentKey == "files") {
		for (const auto& v : values) {
			if (v.type() == typeid(ValueDictionary)) {
				const ValueDictionary& fileDict = boost::get<ValueDictionary>(v);
				TorrentFileInfo file;

				for (const auto& [key, fileValue] : fileDict) {
					currentKey = key;

					if (key == "length") {
						if (fileValue.type() == typeid(int))
							file.length = boost::get<int>(fileValue);
					}
					else if (key == "path") {
						if (fileValue.type() == typeid(ValueVector))
							for (const auto& pathValue : boost::get<ValueVector>(fileValue))
								if (pathValue.type() == typeid(string))
									file.path.push_back(boost::get<string>(pathValue));
					}

					currentKey.clear();
				}

				info.files.push_back(file);
			}
		}
	}
}

void ValueToField::operator()(const ValueDictionary& dict) const {
	if (currentKey == "info") {
		for (const auto& [key, value] : dict) {
			currentKey = key;
			boost::apply_visitor(*this, value);
			currentKey.clear();
		}
	}
}

TorrentFile Torrent::parseTorrentFile(const Value& data) {
	if (data.type() != typeid(ValueDictionary))
		throw std::invalid_argument("Wrong torrent file format");

	ValueDictionary dict = boost::get<ValueDictionary>(data);
	TorrentFile torrent;
	ValueToField visitor(torrent);

	for (const auto& [key, value] : dict) {
		visitor.currentKey = key;
		boost::apply_visitor(visitor, value);
		visitor.currentKey.clear();
	}

	return torrent;
}

Value Torrent::toValue(const TorrentFile& torrent) {
	ValueDictionary result;

	ValueDictionary infoDict;
	infoDict["name"] = torrent.info.name;

	if (!torrent.info.files.empty()) {
		ValueVector filesVector;
		for (const auto& file : torrent.info.files) {
			ValueDictionary fileDict;
			fileDict["length"] = static_cast<int>(file.length);

			ValueVector pathVector;
			for (const auto& pathPart : file.path) 
				pathVector.push_back(pathPart);
			
			fileDict["path"] = pathVector;

			filesVector.push_back(fileDict);
		}
		infoDict["files"] = filesVector;
	}

	infoDict["piece length"] = static_cast<int>(torrent.info.pieceLength);
	infoDict["pieces"] = torrent.info.pieces;
	result["info"] = infoDict;

	ValueVector announceList;
	for (const auto& tier : torrent.announceList) {
		ValueVector tierVector;
		for (const auto& tracker : tier) {
			tierVector.push_back(tracker);
		}
		announceList.push_back(tierVector);
	}
	result["announce-list"] = announceList;

	if (torrent.createdBy) 
		result["created by"] = *torrent.createdBy;


	if (torrent.creationDate) 
		result["creation date"] = static_cast<int>(*torrent.creationDate);


	result["announce"] = torrent.announce;

	return result;
}

TorrentFile Torrent::createTorrentFile(const string& announce, const vector<vector<string>>& announceList, const string& name, const string& folderPath, optional<string> createdBy, optional<time_t> creationDate)
{
	TorrentFile torrent;

	torrent.announce = announce;
	torrent.announceList = announceList;
	torrent.createdBy = createdBy;
	torrent.creationDate = creationDate;
	torrent.info.name = name;

	if (!fs::exists(folderPath))
		throw std::runtime_error("Invalid path: " + folderPath);

	unsigned long long filesSize = 0;

	vector<string> fullPaths;
	vector<TorrentFileInfo> infoFiles;

	if (fs::is_regular_file(folderPath)) {
		TorrentFileInfo cur;
		fs::path filePath = folderPath;
		cur.length = fs::file_size(filePath);

		cur.path.push_back(filePath.filename().string());

		infoFiles.push_back(cur);
		fullPaths.push_back(filePath.string());
		filesSize = cur.length;
	}
	else if (fs::is_directory(folderPath)) {
		for (const auto& entry : fs::recursive_directory_iterator(folderPath)) {
			if (entry.is_regular_file()) {
				TorrentFileInfo cur;

				fs::path filePath = entry.path();
				cur.length = fs::file_size(filePath);

				fs::path relativePath = fs::relative(filePath, folderPath);
				for (const auto& part : relativePath) {
					cur.path.push_back(part.string());
				}

				infoFiles.push_back(cur);
				fullPaths.push_back(filePath.string());
				filesSize += cur.length;
			}
		}

		if (infoFiles.empty()) 
			throw std::runtime_error("The folder is empty: " + folderPath);
	}
	else
		throw std::runtime_error("The path is neither a file nor a directory: " + folderPath);

	torrent.info.pieceLength = Hash::choosePieceLength(filesSize);
	torrent.info.pieces = Hash::createHash(fullPaths, torrent.info.pieceLength);
	torrent.info.files = infoFiles;

	return torrent;
}