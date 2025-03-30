#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include "BitTorrentMessage.h"
#include "PeerRetriever.h"
#include "PieceManager.h"
#include "SharedQueue.h"

using byte = unsigned char;

class PeerConnection
{
private:
    int sock{};
    bool choked{ true };
    bool terminated{ false };
    bool requestPending = { false };
    const std::string clientId;
    const std::string infoHash;
    SharedQueue<Peer*>* _queue;
    Peer* peer;
    std::string peerBitField;
    std::string peerId;
    PieceManager* _piece_manager;

    std::string createHandshakeMessage();
    void performHandshake();
    void receiveBitField();
    void sendInterested();
    void receiveUnchoke();
    void requestPiece();
    void closeSock();
    bool establishNewConnection();
    BitTorrentMessage receiveMessage(int bufferSize = 0) const;

public:
    const std::string& getPeerId() const;

    explicit PeerConnection(SharedQueue<Peer*>* queue, std::string clientId,
        std::string infoHash, PieceManager* piece_manager);
    ~PeerConnection();
    void start();
    void stop();
};


#endif // PEERCONNECTION_H