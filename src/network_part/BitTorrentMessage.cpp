#include "BitTorrentMessage.h"

#include <sstream>

/**
 * An object that represents different types of BitTorrent messages.
 * @param _id: message _id, indicates the type of BitTorrent message.
 * @param _payload: _payload of the message in a string format.
 */

BitTorrentMessage::BitTorrentMessage(const uint8_t id,
                                     const std::string& payload)
    : _id(id), _payload(payload), _message_len(payload.length() + 1)
{
}

std::string BitTorrentMessage::to_string()
{
    std::stringstream buffer;
    char* message_len_addr = (char*)&_message_len;
    std::string message_len_str;
    // Bytes are pushed in reverse order, assuming the data
    // is stored in little-endian order locally.
    for (int i = 0; i < 4; i++)
        message_len_str.push_back((char)message_len_addr[3 - i]);
    buffer << message_len_str;
    buffer << (char)_id;
    buffer << _payload;
    return buffer.str();
}

/**
 * A getter function for the attribute _id.
 * @return the _id of the BitTorrentMessage instance.
 */
uint8_t BitTorrentMessage::message_id() const
{
    return _id;
}

/**
 * A getter function for the attribute _payload.
 * @return the _payload of the BitTorrentMessage instance.
 */
std::string BitTorrentMessage::payload() const
{
    return _payload;
}
