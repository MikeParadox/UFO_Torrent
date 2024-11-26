#include "../includes/torrentFile.h"
#include "torrentFile.h"
#include <fstream>
#include <filesystem>

using std::string;
using std::vector;
using std::optional;

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

string readFile(const std::string& filePath) {
	std::ifstream inputFile(filePath);
	if (!inputFile.is_open())
		throw std::runtime_error("Не удалось открыть файл: " + filePath);


	std::string content((std::istreambuf_iterator<char>(inputFile)),
		std::istreambuf_iterator<char>());

	inputFile.close();

	return content;
}

Value Torrent::toValue(const TorrentFile& torrent) {
	ValueDictionary result;

	result["announce"] = torrent.announce;


	ValueVector announceList;
	for (const auto& tier : torrent.announceList) {
		ValueVector tierVector;
		for (const auto& tracker : tier) {
			tierVector.push_back(tracker);
		}
		announceList.push_back(tierVector);
	}
	result["announce-list"] = announceList;

	if (torrent.createdBy) {
		result["created by"] = *torrent.createdBy;
	}

	if (torrent.creationDate) {
		result["creation date"] = static_cast<int>(*torrent.creationDate);
	}

	ValueDictionary infoDict;
	infoDict["name"] = torrent.info.name;
	infoDict["piece length"] = static_cast<int>(torrent.info.pieceLength);
	infoDict["pieces"] = torrent.info.pieces;

	if (!torrent.info.files.empty()) {
		ValueVector filesVector;
		for (const auto& file : torrent.info.files) {
			ValueDictionary fileDict;
			fileDict["length"] = static_cast<int>(file.length);

			ValueVector pathVector;
			for (const auto& pathPart : file.path) {
				pathVector.push_back(pathPart);
			}
			fileDict["path"] = pathVector;

			filesVector.push_back(fileDict);
		}
		infoDict["files"] = filesVector;
	}

	result["info"] = infoDict;

	return result;
}
