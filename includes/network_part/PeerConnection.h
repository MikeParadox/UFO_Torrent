/**
 * @file PeerConnection.h
 * @brief Handles connection and communication with a single peer.
 * @details Manages the peer connection lifecycle including handshake,
 *          message exchange and piece requests.
 */
#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include "BitTorrentMessage.h"
#include "PeerRetriever.h"
#include "PieceManager.h"
#include "SharedQueue.h"

using byte = unsigned char;

/**
 * @class PeerConnection
 * @brief Represents a connection to a peer in the BitTorrent network.
 */
class PeerConnection
{
private:
    int sock{};                     /**< Socket descriptor. */
    bool choked{ true };            /**< Choke state from peer. */
    bool terminated{ false };       /**< Termination flag. */
    bool requestPending = { false }; /**< Request pending flag. */
    const std::string clientId;     /**< Our client ID. */
    const std::string infoHash;     /**< Torrent info hash. */
    SharedQueue<Peer*>* _queue;     /**< Queue of available peers. */
    Peer* peer;                     /**< Current peer. */
    std::string peerBitField;       /**< Peer's bitfield of pieces. */
    std::string peerId;             /**< Peer's ID. */
    PieceManager* _piece_manager;   /**< Piece manager reference. */

    /**
     * @brief Creates handshake message.
     * @return Handshake message string.
     */
    std::string createHandshakeMessage();

    /**
     * @brief Performs handshake with peer.
     */
    void performHandshake();

    /**
     * @brief Receives peer's bitfield.
     */
    void receiveBitField();

    /**
     * @brief Sends interested message to peer.
     */
    void sendInterested();

    /**
     * @brief Receives unchoke message from peer.
     */
    void receiveUnchoke();

    /**
     * @brief Requests a piece from peer.
     */
    void requestPiece();

    /**
     * @brief Closes connection socket.
     */
    void closeSock();

    /**
     * @brief Establishes new connection to peer.
     * @return true if successful, false otherwise.
     */
    bool establishNewConnection();

    /**
     * @brief Receives message from peer.
     * @param bufferSize Buffer size for receiving.
     * @return Received BitTorrent message.
     */
    BitTorrentMessage receiveMessage(int bufferSize = 0) const;

public:
    /**
         * @brief Gets peer ID.
         * @return Peer's ID string.
         */
    const std::string& getPeerId() const;

    /**
     * @brief Constructs PeerConnection.
     * @param queue Shared queue of peers.
     * @param clientId Our client ID.
     * @param infoHash Torrent info hash.
     * @param piece_manager Piece manager instance.
     */
    explicit PeerConnection(SharedQueue<Peer*>* queue, std::string clientId,
        std::string infoHash, PieceManager* piece_manager);
    /**
    * @brief Destructs PeerConnection.
    */
    ~PeerConnection();

    /**
     * @brief Starts peer connection and download.
     */
    void start();

    /**
     * @brief Stops peer connection.
     */
    void stop();
};


#endif // PEERCONNECTION_H