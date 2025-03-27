#include "TorrentClient.h"
#include "PeerConnection.h"
#include "PeerRetriever.h"
#include "PieceManager.h"
#include "TorrentFileParser.h"
#include "bencoding.h"
#include <iostream>
#include <random>
#include <thread>


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
    // Parse Torrent file
    std::cout << "Parsing Torrent file " + torrentFilePath + "..." << std::endl;
    TorrentFileParser torrentFileParser(torrentFilePath);
    std::string announceUrl = torrentFileParser.getAnnounce();

    long fileSize = torrentFileParser.getFileSize();
    const std::string infoHash = torrentFileParser.getInfoHash();

    std::string filename = torrentFileParser.getFileName();
    std::string downloadPath = downloadDirectory + filename;
    PieceManager pieceManager(torrentFileParser, downloadPath, threadNum);

    // Adds threads to the thread pool
    for (int i = 0; i < threadNum; i++)
    {
        PeerConnection connection(&queue, peerId, infoHash, &pieceManager);
        connections.push_back(std::make_unique<PeerConnection>(connection));
        std::thread thread(&PeerConnection::start, connection);
        threadPool.push_back(std::move(thread));
    }

    auto lastPeerQuery = (time_t)(-1);

    std::cout << "Download initiated..." << std::endl;

    while (true)
    {
        if (pieceManager.isComplete()) break;

        time_t currentTime = std::time(nullptr);
        auto diff = std::difftime(currentTime, lastPeerQuery);
        // Retrieve peers from the tracker after a certain time interval or
        // whenever the queue is empty
        if (lastPeerQuery == -1 || diff >= PEER_QUERY_INTERVAL || queue.empty())
        {
            PeerRetriever peerRetriever(peerId, announceUrl, infoHash, PORT,
                                        fileSize);
            std::vector<Peer*> peers =
                peerRetriever.retrievePeers(pieceManager.bytesDownloaded());
            lastPeerQuery = currentTime;
            if (!peers.empty())
            {
                queue.clear();
                for (auto peer : peers) queue.push_back(peer);
            }
        }
    }

    terminate();

    if (pieceManager.isComplete())
    {
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
