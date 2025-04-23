/**
 * @file BitTorrentMessage.h
 * @brief Defines BitTorrent protocol message types and base message class.
 * @details Contains enum with message IDs and class representing a BitTorrent protocol message.
 */
#ifndef BIT_TORRENT_MESSAGE_H
#define BIT_TORRENT_MESSAGE_H

#include <cstdint>
#include <string>


 /**
  * @enum MessageId
  * @brief Enumeration of BitTorrent protocol message types.
  */
enum MessageId
{
    keepAlive = -1,     /**< Keep-alive message (no payload). */
    choke = 0,          /**< Choke message (peer is choking). */
    unchoke = 1,        /**< Unchoke message (peer is unchoking). */
    interested = 2,     /**< Interested message (peer is interested). */
    notInterested = 3,  /**< Not interested message (peer is not interested). */
    have = 4,          /**< Have message (peer has a piece). */
    bitField = 5,      /**< Bitfield message (peer's available pieces). */
    request = 6,       /**< Request message (requesting a block). */
    piece = 7,         /**< Piece message (sending a block). */
    cancel = 8,        /**< Cancel message (canceling a request). */
    port = 9           /**< Port message (DHT port announcement). */
};

/**
 * @class BitTorrentMessage
 * @brief Represents a BitTorrent protocol message.
 */
class BitTorrentMessage
{
private:
    const uint32_t _message_len; /**< Length of the message. */
    const uint8_t _id;          /**< Message type ID. */
    const std::string _payload; /**< Message payload data. */

public:
    /**
         * @brief Constructs a BitTorrent message.
         * @param _id Message type ID.
         * @param _payload Message payload (default empty).
         */
    explicit BitTorrentMessage(uint8_t _id, const std::string& _payload = "");
    /**
     * @brief Converts message to string representation.
     * @return String representation of the message.
     */
    std::string to_string();
    /**
     * @brief Gets message ID.
     * @return Message type ID.
     */
    uint8_t message_id() const;

    /**
     * @brief Gets message payload.
     * @return Message payload data.
     */
    std::string payload() const;
};

#endif
