#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include "PeerConnection.h"
#include "PeerRetriever.h"
#include "SharedQueue.h"
#include <string>
#include <thread>

class TorrentClient
{
private:
    const int threadNum;
    std::string peerId;
    SharedQueue<Peer*> queue;
    std::vector<std::thread> threadPool;
    std::vector<std::unique_ptr<PeerConnection>> connections;

public:
    explicit TorrentClient(int threadNum = 5, bool enableLogging = true,
                           std::string logFilePath = "logs/client.log");
    ~TorrentClient();
    void terminate();
    void downloadFile(const std::string& torrentFilePath,
                      const std::string& downloadDirectory);
};

#endif // BITTORRENTCLIENT_TORRENTCLIENT_H
