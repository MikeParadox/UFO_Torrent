/**
 * @file PieceManager.h
 * @brief Manages torrent pieces and download process.
 * @details Handles piece allocation, peer management, and download progress tracking.
 */
#ifndef PIECEMANAGER_H
#define PIECEMANAGER_H

#include "Piece.h"
#include "torrentFile.h"  // Âàø íîâûé çàãîëîâî÷íûé ôàéë

#include <ctime>
#include <fstream>
#include <map>
#include <mutex>
#include <vector>

 /**
  * @struct PendingRequest
  * @brief Represents a pending block request to a peer.
  */
struct PendingRequest {
    Block* block;       /**< Requested block pointer. */
    time_t timestamp;   /**< Request timestamp. */
};

/**
 * @class PieceManager
 * @brief Core class managing torrent piece download strategy.
 */
class PieceManager {
private:
    std::map<std::string, std::string> peers;       /**< Map of peer IDs to their bitfields. */
    std::vector<Piece*> missingPieces;              /**< Pieces not yet downloaded. */
    std::vector<Piece*> ongoingPieces;              /**< Pieces currently being downloaded. */
    std::vector<Piece*> havePieces;                 /**< Pieces successfully downloaded. */
    std::vector<PendingRequest*> pendingRequests;   /**< Pending block requests. */
    std::ofstream downloadedFile;                   /**< Output file stream. */
    const long pieceLength;                         /**< Length of each piece in bytes. */
    const Torrent::TorrentFile& torrentFile;        /**< Reference to torrent metadata. */
    const int maximumConnections;                   /**< Max simultaneous connections. */
    int piecesDownloadedInInterval = 0;             /**< Pieces downloaded in current interval. */
    time_t startingTime;                            /**< Download start timestamp. */
    int totalPieces{};                              /**< Total pieces in torrent. */
    std::mutex lock;                                /**< Mutex for thread safety. */

    /**
     * @brief Initializes piece structures from torrent file.
     * @return Vector of Piece pointers.
     */
    std::vector<Piece*> initiatePieces();

    /**
     * @brief Checks for expired requests.
     * @param peerId Peer ID to check.
     * @return Expired Block or nullptr.
     */
    Block* expiredRequest(const std::string& peerId);

    /**
     * @brief Gets next block from ongoing pieces.
     * @param peerId Peer ID making the request.
     * @return Next Block or nullptr.
     */
    Block* nextOngoing(const std::string& peerId);

    /**
     * @brief Implements rarest-first piece selection.
     * @param peerId Peer ID making the request.
     * @return Rarest available Piece.
     */
    Piece* getRarestPiece(const std::string& peerId);

    /**
     * @brief Writes completed piece to disk.
     * @param piece Completed piece.
     */
    void write(Piece* piece);

    /**
     * @brief Displays progress bar in console.
     */
    void displayProgressBar();

    /**
     * @brief Tracks download speed and ETA.
     */
    void trackProgress();

    // Âñïîìîãàòåëüíûå ìåòîäû äëÿ ðàáîòû ñ TorrentFile
    long getTotalFileSize() const;
    std::vector<std::string> splitPieceHashes() const;

public:
    /**
         * @brief Constructs PieceManager.
         * @param torrentFile Torrent metadata reference.
         * @param downloadPath Output file path.
         * @param maximumConnections Max connections allowed.
         */
    explicit PieceManager(const Torrent::TorrentFile& torrentFile,
        const std::string& downloadPath,
        int maximumConnections);

    /**
    * @brief Destructs PieceManager.
    */
    ~PieceManager();

    /**
     * @brief Checks if download is complete.
     * @return true if all pieces downloaded.
     */
    bool isComplete();

    /**
     * @brief Handles received block.
     * @param peerId Sender peer ID.
     * @param pieceIndex Piece index.
     * @param blockOffset Block offset.
     * @param data Block data.
     */
    void blockReceived(std::string peerId, int pieceIndex, int blockOffset,
        std::string data);

    /**
    * @brief Adds new peer.
    * @param peerId Peer ID.
    * @param bitField Peer's bitfield.
    */
    void addPeer(const std::string& peerId, const std::string& bitField);

    /**
     * @brief Removes disconnected peer.
     * @param peerId Peer ID to remove.
     */
    void removePeer(const std::string& peerId);

    /**
     * @brief Updates peer's bitfield.
     * @param peerId Peer ID.
     * @param index Piece index peer now has.
     */
    void updatePeer(const std::string& peerId, int index);

    /**
     * @brief Gets total bytes downloaded.
     * @return Bytes downloaded.
     */
    unsigned long bytesDownloaded();

    /**
     * @brief Gets next block to request.
     * @param peerId Peer ID making the request.
     * @return Block to request or nullptr.
     */
    Block* nextRequest(const std::string& peerId);
};

#endif // PIECEMANAGER_H