#ifndef TORRENTFILE_H
#define TORRENTFILE_H

#include <boost/variant.hpp>
#include <ctime>
#include <optional>
#include <stdexcept>
#include "valueTypes.h"

using std::string;
using std::vector;
using std::optional;

using namespace bencode;

namespace Torrent {

    // Структура для представления файлов внутри торрента
    struct TorrentFileInfo {
        unsigned long long length;                      // Размер файла в байтах
        vector<string> path;                            // Путь к файлу (например, ["data.bin"])
    };

    // Основная структура для описания торрента
    struct TorrentFile {

        string announce;                                // Основной трекер
        vector<vector<string>> announceList;            // Альтернативные трекеры
        optional<string> createdBy;                     // Кто создал (если есть)
        optional<time_t> creationDate;                  // Дата создания (UNIX Timestamp)

        struct Info {
            vector<TorrentFileInfo> files;              // Список файлов
            string name;                                // Имя файла или папки
            unsigned long long pieceLength;             // Размер одной части (в байтах)
            string pieces;                              // Хеши всех частей (бинарные данные)
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

    // Функция для преобразования value в TorrentFile
    TorrentFile parseTorrentFile(const Value& data);
    Value toValue(const TorrentFile& torrent);
};

string readFile(const std::string& filePath);

#endif