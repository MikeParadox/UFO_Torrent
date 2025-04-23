/**
 * @file TorrentClient.h
 * @brief Main torrent client class.
 * @details Coordinates peer connections and download process.
 */
#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include "PeerConnection.h"
#include "PeerRetriever.h"
#include "SharedQueue.h"
#include <string>
#include <thread>

 /**
  * @class TorrentClient
  * @brief Main client class managing the torrent download.
  */
class TorrentClient
{
private:
    const int threadNum; /**< Number of worker threads. */
    std::string peerId; /**< Our peer ID. */
    SharedQueue<Peer*> queue; /**< Queue of available peers. */
    std::vector<std::thread> threadPool; /**< Worker thread pool. */
    std::vector<std::unique_ptr<PeerConnection>> connections; /**< Active connections. */

public:
    /**
         * @brief Constructs TorrentClient.
         * @param threadNum Number of worker threads.
         * @param enableLogging Enable logging flag.
         * @param logFilePath Log file path.
         */
    explicit TorrentClient(int threadNum = 5, bool enableLogging = true,
        std::string logFilePath = "logs/client.log");
    /**
     * @brief Destructs TorrentClient.
     */
    ~TorrentClient();
    /**
     * @brief Stops all downloads and cleans up.
     */
    void terminate();

    /**
     * @brief Starts torrent download.
     * @param torrentFilePath Path to .torrent file.
     * @param downloadDirectory Output directory.
     */
    void downloadFile(const std::string& torrentFilePath,
        const std::string& downloadDirectory);
};

#endif // BITTORRENTCLIENT_TORRENTCLIENT_H
