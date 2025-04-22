/**
 * @file PeerRetriever.h
 * @brief Retrieves peer list from tracker.
 * @details Handles communication with tracker and peer list decoding.
 */
#ifndef PEER_RETRIEVER_H
#define PEER_RETRIEVER_H

#include <string>
#include <vector>

 /**
  * @struct Peer
  * @brief Represents a peer with IP and port.
  */
struct Peer
{
    std::string ip; /**< Peer IP address. */
    int port;       /**< Peer port number. */
};

/**
 * @class PeerRetriever
 * @brief Retrieves peers list from tracker.
 */
class PeerRetriever
{
private:
    std::string announceUrl;    /**< Tracker announce URL. */
    std::string infoHash;       /**< Torrent info hash. */
    std::string peerId;         /**< Our peer ID. */
    int port;                   /**< Our listening port. */
    const unsigned long fileSize; /**< Total file size. */

    /**
     * @brief Decodes tracker response.
     * @param response Tracker response string.
     * @return Vector of Peer pointers.
     */
    std::vector<Peer*> decodeResponse(std::string response);

public:
    /**
         * @brief Constructs PeerRetriever.
         * @param peerId Our peer ID.
         * @param announceUrL Tracker URL.
         * @param infoHash Torrent info hash.
         * @param port Our listening port.
         * @param fileSize Total file size.
         */
    explicit PeerRetriever(std::string peerId, std::string announceUrL,
        std::string infoHash, int port,
        unsigned long fileSize);
    /**
     * @brief Retrieves peers from tracker.
     * @param bytesDownloaded Bytes downloaded so far.
     * @return Vector of Peer pointers.
     */
    std::vector<Peer*> retrievePeers(unsigned long bytesDownloaded = 0);
};

#endif // BITTORRENTCLIENT_PEERRETRIEVER_H
