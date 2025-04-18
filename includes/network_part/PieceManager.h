// PieceManager.h
#ifndef PIECEMANAGER_H
#define PIECEMANAGER_H

#include "Piece.h"
#include "torrentFile.h"  // Âàø íîâûé çàãîëîâî÷íûé ôàéë

#include <ctime>
#include <fstream>
#include <map>
#include <mutex>
#include <vector>

struct PendingRequest {
    Block* block;
    time_t timestamp;
};

class PieceManager {
private:
    std::map<std::string, std::string> peers;
    std::vector<Piece*> missingPieces;
    std::vector<Piece*> ongoingPieces;
    std::vector<Piece*> havePieces;
    std::vector<PendingRequest*> pendingRequests;
    std::ofstream downloadedFile;
    const long pieceLength;
    const Torrent::TorrentFile& torrentFile;  // Èñïîëüçóåì TorrentFile âìåñòî TorrentFileParser
    const int maximumConnections;
    int piecesDownloadedInInterval = 0;
    time_t startingTime;
    int totalPieces{};
    std::mutex lock;

    std::vector<Piece*> initiatePieces();
    Block* expiredRequest(const std::string& peerId);
    Block* nextOngoing(const std::string& peerId);
    Piece* getRarestPiece(const std::string& peerId);
    void write(Piece* piece);
    void displayProgressBar();
    void trackProgress();

    // Âñïîìîãàòåëüíûå ìåòîäû äëÿ ðàáîòû ñ TorrentFile
    long getTotalFileSize() const;
    std::vector<std::string> splitPieceHashes() const;

public:
    explicit PieceManager(const Torrent::TorrentFile& torrentFile,
        const std::string& downloadPath,
        int maximumConnections);
    ~PieceManager();
    bool isComplete();
    void blockReceived(std::string peerId, int pieceIndex, int blockOffset,
        std::string data);
    void addPeer(const std::string& peerId, const std::string& bitField);
    void removePeer(const std::string& peerId);
    void updatePeer(const std::string& peerId, int index);
    unsigned long bytesDownloaded();
    Block* nextRequest(const std::string& peerId);
};

#endif // PIECEMANAGER_H