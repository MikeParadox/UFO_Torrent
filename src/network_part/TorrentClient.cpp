#include "TorrentClient.h"
#include "PeerConnection.h"
#include "PeerRetriever.h"
#include "PieceManager.h"
#include "TorrentFileParser.h"
#include "torrentFile.h"
#include "fileUtils.h"
#include "decode.h"
#include "bencoding.h"
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <iostream>
#include <random>
#include <thread>
#include <algorithm>

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

#define PORT 8080
#define PEER_QUERY_INTERVAL 60

TorrentClient::TorrentClient(const int threadNum, bool enableLogging,
                             std::string logFilePath)
    : threadNum(threadNum)
{
    // Generate a random 20-byte peer Id for the client as per the convention
    peerId = "-UT2021-";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, 9);
    for (int i = 0; i < 12; i++) peerId += std::to_string(distrib(gen));
}

/**
 * Ensures that all resources are freed when TorrentClient is destroyed
 */
TorrentClient::~TorrentClient() = default;

/**
 * Download the file as per the content of the given Torrent file.
 * @param torrentFilePath: path to the Torrent file.
 * @param downloadPath: directory of the file when it is finished (i.e. the
 * destination directory).
 */

void TorrentClient::downloadFile(const std::string& torrentFilePath,
    const std::string& downloadDirectory)
{
    // Чтение torrent файла в строку
    std::string torrentData = read(torrentFilePath);

    // Парсинг torrent файла с использованием libtorrent
    std::cout << "Parsing Torrent file " + torrentFilePath + "..." << std::endl;
    lt::error_code ec;
    lt::torrent_info ti(torrentData.c_str(), static_cast<int>(torrentData.size()), ec);
    if (ec) {
        std::cerr << "Failed to parse torrent file: " << ec.message() << std::endl;
        return;
    }

    // Получение info_hash из libtorrent правильным образом
    lt::sha1_hash info_hash = ti.info_hash();
    std::string infoHash(info_hash.data(), info_hash.size()); // Бинарный хеш для handshake

    // URL-encoded хеш для трекера
    std::string urlEncodedHash;
    for (int i = 0; i < 20; ++i) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%%%02X", static_cast<unsigned char>(info_hash[i]));
        urlEncodedHash += buf;
    }

    // Используем существующий парсер для остальных данных
    Torrent::TorrentFile file = Torrent::parseTorrentFile(Decoder::decode(torrentData));
    std::string announceUrl;

    // 1. Проверяем основное поле announce
    if (!file.announce.empty() &&
        (file.announce.find("http://") == 0 || file.announce.find("https://") == 0))
    {
        announceUrl = file.announce;
    }
    // 2. Если announce невалидный, ищем в announceList
    else if (!file.announceList.empty())
    {
        for (const auto& tier : file.announceList) {
            for (const auto& url : tier) {
                if (url.find("http://") == 0 || url.find("https://") == 0) {
                    announceUrl = url;
                    break;
                }
            }
            if (!announceUrl.empty()) break;
        }
    }

    std::cout << "Announce URL: " << announceUrl << std::endl;
    std::cout << "Info Hash: " << info_hash.to_string() << std::endl;

    // Расчет общего размера файлов
    long fileSize = ti.total_size(); // Используем libtorrent для получения размера

    std::string filename = ti.name(); // Используем libtorrent для получения имени
    std::string downloadPath = downloadDirectory + filename;

    // Инициализация PieceManager с использованием libtorrent info_hash
    PieceManager pieceManager(file, downloadPath, threadNum);

    // Создание потоков
    for (int i = 0; i < threadNum; i++) {
        PeerConnection connection(&queue, peerId, infoHash, &pieceManager);
        connections.push_back(std::make_unique<PeerConnection>(connection));
        std::thread thread(&PeerConnection::start, connection);
        threadPool.push_back(std::move(thread));
    }

    // Остальной код без изменений
    auto lastPeerQuery = (time_t)(-1);
    std::cout << "Download initiated..." << std::endl;

    while (true) {
        if (pieceManager.isComplete()) break;

        time_t currentTime = std::time(nullptr);
        auto diff = std::difftime(currentTime, lastPeerQuery);
        if (lastPeerQuery == -1 || diff >= PEER_QUERY_INTERVAL || queue.empty()) {
            PeerRetriever peerRetriever(peerId, announceUrl, urlEncodedHash, PORT, fileSize);
            std::vector<Peer*> peers = peerRetriever.retrievePeers(pieceManager.bytesDownloaded());
            lastPeerQuery = currentTime;
            if (!peers.empty()) {
                queue.clear();
                for (auto peer : peers) queue.push_back(peer);
            }
        }

        // Отладочное сообщение для отслеживания прогресса
    }

    terminate();

    if (pieceManager.isComplete()) {
        std::cout << "Download completed!" << std::endl;
        std::cout << "File downloaded to " << downloadPath << std::endl;
    }
}


/**
 * Terminates the download and cleans up all the resources
 */
void TorrentClient::terminate()
{
    // Pushes dummy Peers into the queue so that
    // the waiting threads can terminate
    for (int i = 0; i < threadNum; i++)
    {
        Peer* dummyPeer = new Peer{"0.0.0.0", 0};
        queue.push_back(dummyPeer);
    }
    // for (auto connection : connections) connection->stop();
    std::for_each(connections.begin(), connections.end(),
                  [](auto& x) { x->stop(); });


    for (std::thread& thread : threadPool)
    {
        if (thread.joinable()) thread.join();
    }

    threadPool.clear();
}
