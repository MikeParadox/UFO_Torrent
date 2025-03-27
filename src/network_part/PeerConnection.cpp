#include "PeerConnection.h"
#include "connect.h"
#include "utils.h"
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>


constexpr int info_hash_starting_pos{28};
constexpr int peer_id_starting_pos{48};
constexpr int hash_len{20};
std::string dummy_peer_ip{"0.0.0.0"};

/**
 * Constructor of the class PeerConnection.
 * @param queue: the thread-safe queue that contains the available peers.
 * @param clientId: the peer ID of this C++ BitTorrent client. Generated in the
 * TorrentClient class.
 * @param infoHash: info hash of the Torrent file.
 * @param pieceManager: pointer to the PieceManager.
 */
PeerConnection::PeerConnection(SharedQueue<Peer*>* queue, std::string clientId,
                               std::string infoHash,
                               PieceManager* piece_manager)
    : _queue(queue), clientId(std::move(clientId)),
      infoHash(std::move(infoHash)), _piece_manager(piece_manager)
{
}


/**
 * Destructor of the PeerConnection class. Closes the established TCP connection
 * with the peer on object destruction.
 */
PeerConnection::~PeerConnection()
{
    closeSock();
}

void PeerConnection::start()
{
    while (!(terminated || _piece_manager->isComplete()))
    {
        peer = _queue->pop_front();
        // Terminates the thread if it has received a dummy Peer
        if (peer->ip == dummy_peer_ip) return;

        try
        {
            // Establishes connection with the peer, and lets it know
            // that we are interested.
            if (establishNewConnection())
            {
                while (!_piece_manager->isComplete())
                {
                    BitTorrentMessage message = receiveMessage();
                    if (message.message_id() > 10)
                        throw std::runtime_error(
                            "Received invalid message Id from peer " + peerId);
                    switch (message.message_id())
                    {
                    case choke: choked = true; break;

                    case unchoke: choked = false; break;

                    case piece:
                    {
                        requestPending = false;
                        std::string payload = message.payload();
                        int index = bytesToInt(payload.substr(0, 4));
                        int begin = bytesToInt(payload.substr(4, 4));
                        std::string blockData = payload.substr(8);
                        _piece_manager->blockReceived(peerId, index, begin,
                                                      blockData);
                        break;
                    }
                    case have:
                    {
                        std::string payload = message.payload();
                        int pieceIndex = bytesToInt(payload);
                        _piece_manager->updatePeer(peerId, pieceIndex);
                        break;
                    }

                    default: break;
                    }
                    if (!choked)
                    {
                        if (!requestPending) { requestPiece(); }
                    }
                }
            }
        }
        catch (std::exception& e)
        {
            closeSock();
        }
    }
}

/**
 * Terminates the peer connection
 */
void PeerConnection::stop()
{
    terminated = true;
}


/**
 * Establishes a TCP connection with the peer and sent it our initial BitTorrent
 * handshake message. Waits for its reply, and compares the info hash contained
 * in its response message with the info hash we calculated from the Torrent
 * file. If they do not match, close the connection.
 */
void PeerConnection::performHandshake()
{
    // Connects to the peer
    try
    {
        sock = createConnection(peer->ip, peer->port);
    }
    catch (std::runtime_error& e)
    {
        throw std::runtime_error("Cannot connect to peer [" + peer->ip + "]");
    }

    // Send the handshake message to the peer
    std::string handshakeMessage = createHandshakeMessage();
    sendData(sock, handshakeMessage);

    // Receive the reply from the peer
    std::string reply = receiveData(sock, handshakeMessage.length());
    if (reply.empty())
        throw std::runtime_error(
            "Receive handshake from peer: FAILED [No response from peer]");
    peerId = reply.substr(info_hash_starting_pos, hash_len);

    // Compare the info hash from the peer's reply message with the info hash we
    // sent. If the two values are not the same, close the connection and raise
    // an exception.
    std::string receivedInfoHash =
        reply.substr(info_hash_starting_pos, hash_len);
    if ((receivedInfoHash == infoHash) != 0)
        throw std::runtime_error("Perform handshake with peer " + peer->ip +
                                 ": FAILED [Received mismatching info hash]");
}

/**
 * Receives and reads the message which contains BitField from the peer.
 */
void PeerConnection::receiveBitField()
{
    // Receive BitField from the peer
    BitTorrentMessage message = receiveMessage();
    if (message.message_id() != bitField)
        throw std::runtime_error(
            "Receive BitField from peer: FAILED [Wrong message ID]");
    peerBitField = message.payload();

    // Informs the PieceManager of the BitField received
    _piece_manager->addPeer(peerId, peerBitField);
}

/**
 * Sends a request message to the peer for the next block
 * to be downloaded.
 */
void PeerConnection::requestPiece()
{
    Block* block = _piece_manager->nextRequest(peerId);

    if (!block) return;

    int payloadLength = 12;
    char temp[payloadLength];
    // Needs to convert little-endian to big-endian
    uint32_t index = htonl(block->piece);
    uint32_t offset = htonl(block->offset);
    uint32_t length = htonl(block->length);
    std::memcpy(temp, &index, sizeof(int));
    std::memcpy(temp + 4, &offset, sizeof(int));
    std::memcpy(temp + 8, &length, sizeof(int));
    std::string payload;
    for (int i = 0; i < payloadLength; i++) payload += (char)temp[i];

    std::stringstream info;
    info << "Sending Request message to peer " << peer->ip << " ";
    info << "[Piece: " << std::to_string(block->piece) << " ";
    info << "Offset: " << std::to_string(block->offset) << " ";
    info << "Length: " << std::to_string(block->length) << "]";
    std::string requestMessage =
        BitTorrentMessage(request, payload).to_string();
    sendData(sock, requestMessage);
    requestPending = true;
}

/**
 * Send an Interested message to the peer.
 */
void PeerConnection::sendInterested()
{
    std::string interestedMessage = BitTorrentMessage(interested).to_string();
    sendData(sock, interestedMessage);
}

/**
 * Receives and reads the Unchoke message from the peer.
 * If the received message does not match the expected Unchoke, raise an error.
 */
void PeerConnection::receiveUnchoke()
{
    BitTorrentMessage message = receiveMessage();
    if (message.message_id() != unchoke)
        throw std::runtime_error(
            "Receive Unchoke message from peer: FAILED [Wrong message ID: " +
            std::to_string(message.message_id()) + "]");
    choked = false;
}

/**
 * This function establishes a TCP connection with the peer and performs
 * the following actions:
 *
 * 1. Sends the peer a BitTorrent handshake message, waits for its reply and
 * compares the info hashes.
 * 2. Receives and stores the BitField from the peer.
 * 3. Send an Interested message to the peer.
 *
 * Returns true if a stable connection has been successfully established,
 * false otherwise.
 *
 * To understand the details, the following links can be helpful:
 * - https://blog.jse.li/posts/torrent/
 * - https://markuseliasson.se/article/bittorrent-in-python/
 * - https://wiki.theory.org/BitTorrentSpecification#Handshake
 */
bool PeerConnection::establishNewConnection()
{
    try
    {
        performHandshake();
        receiveBitField();
        sendInterested();
        return true;
    }
    catch (const std::runtime_error& e)
    {
        return false;
    }
}

/**
 * Generates the initial handshake message to send to the peer.
 * Essentially, the handshake message has the following structure:
 *
 * handshake: <pstrlen><pstr><reserved><info_hash><peer_id>
 * pstrlen: string length of <pstr>, as a single raw byte
 * pstr: string identifier of the protocol
 * reserved: eight (8) reserved bytes.
 * info_hash: 20-byte SHA1 hash Torrent file.
 * peer_id: 20-byte string used as a unique ID for the client.
 *
 * @return a string representation of the Torrent handshake message.
 */
std::string PeerConnection::createHandshakeMessage()
{
    const std::string protocol = "BitTorrent protocol";
    std::stringstream buffer;
    buffer << (char)protocol.length();
    buffer << protocol;
    std::string reserved;
    for (int i = 0; i < 8; i++) reserved.push_back('\0');
    buffer << reserved;
    buffer << hexDecode(infoHash);
    buffer << clientId;
    assert(buffer.str().length() == protocol.length() + 49);
    return buffer.str();
}


/**
 * A wrapper around the receiveData() function, in a sense that it returns
 * a BitTorrentMessage object so that parameters such as message length, id
 * and payload can be accessed more easily.
 */
BitTorrentMessage PeerConnection::receiveMessage(int bufferSize) const
{
    std::string reply = receiveData(sock, 0);
    if (reply.empty()) return BitTorrentMessage(keepAlive);
    auto messageId = (uint8_t)reply[0];
    std::string payload = reply.substr(1);

    return BitTorrentMessage(messageId, payload);
}

/**
 * Retrieves the peer ID of the peer that is currently in contact with us.
 */
const std::string& PeerConnection::getPeerId() const
{
    return peerId;
}

/**
 * Closes the socket to a peer and sets the
 * instance variable 'sock' to null;
 */
void PeerConnection::closeSock()
{
    if (sock)
    {
        // Close socket
        close(sock);
        sock = {};
        requestPending = false;
        // If the peer has been added to piece manager, remove it
        if (!peerBitField.empty())
        {
            peerBitField.clear();
            _piece_manager->removePeer(peerId);
        }
    }
}
