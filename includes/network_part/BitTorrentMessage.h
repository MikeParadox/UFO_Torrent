#ifndef BIT_TORRENT_MESSAGE_H
#define BIT_TORRENT_MESSAGE_H

#include <cstdint>
#include <string>


enum MessageId
{
    keepAlive = -1,
    choke = 0,
    unchoke = 1,
    interested = 2,
    notInterested = 3,
    have = 4,
    bitField = 5,
    request = 6,
    piece = 7,
    cancel = 8,
    port = 9
};

class BitTorrentMessage
{
private:
    const uint32_t _message_len;
    const uint8_t _id;
    const std::string _payload;

public:
    explicit BitTorrentMessage(uint8_t _id, const std::string& _payload = "");
    std::string to_string();
    uint8_t message_id() const;
    std::string payload() const;
};

#endif
