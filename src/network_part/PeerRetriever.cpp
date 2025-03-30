#include "PeerRetriever.h"
#include "BItem.h"
#include "BString.h"
#include "utils.h"
#include <bencoding.h>
#include <curl/curl.h>
#include <iostream>
#include "valueTypes.h"
#include "decode.h"
#include <sstream>
#include <libtorrent/session.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/hex.hpp> // for hex decoding
#include <libtorrent/aux_/escape_string.hpp> // for from_hex
#include <libtorrent/random.hpp> // for random peer_id generation
#include <stdexcept>
#include <utility>

constexpr int tracker_timeout{ 15000 };

/**
 * Constructor of the class PeerRetriever. Takes in the URL as specified by the
 * value of announce in the Torrent file, the info hash of the file, as well as
 * a port number.
 * @param announceURL: the HTTP URL to the tracker.
 * @param infoHash: the info hash of the Torrent file.
 * @param port: the TCP port this client listens on.
 * @param fileSize: the size of the file to be downloaded in bytes.
 */
PeerRetriever::PeerRetriever(std::string peerId, std::string announceUrl,
    std::string infoHash, int port,
    const unsigned long fileSize)
    : fileSize(fileSize)
{

    this->peerId = std::move(peerId);
    this->announceUrl = std::move(announceUrl);
    this->infoHash = infoHash;
    this->port = port;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::vector<Peer*> PeerRetriever::retrievePeers(unsigned long bytesDownloaded) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<Peer*> peers;

    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize cURL");
    }

    // Формируем URL с параметрами
    std::ostringstream urlStream;
    urlStream << announceUrl
        << "?info_hash=" << infoHash
        << "&peer_id=" << peerId
        << "&port=" << port
        << "&uploaded=0"
        << "&downloaded=" << bytesDownloaded
        << "&left=" << (fileSize - bytesDownloaded)
        << "&compact=1";

    std::string url = urlStream.str();

    // Настраиваем параметры cURL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, tracker_timeout / 1000); // в секундах
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Разрешаем редиректы
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libtorrent/1.2.0"); // Устанавливаем User-Agent

    // Выполняем запрос
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
        return peers;
    }

    // Получаем код ответа HTTP
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    std::cout << "Peers found " << std::endl;

    if (http_code == 200) {
        peers = decodeResponse(readBuffer);
        std::cout << "Number of peers found: " << peers.size() << std::endl;
    }
    else {
        std::cerr << "Failed to retrieve peers, HTTP code: " << http_code << std::endl;
    }

    curl_easy_cleanup(curl);
    return peers;
}

/**
 * Decodes the response string sent by the tracker. If the string can
 * successfully decoded, returns a list of pointers to Peer structs. Note that
 * this functions handles two distinct representations, one of them has the
 * peers denoted as a long binary blob (compact), the other represents peers in
 * a list with all the information already in place. The former can be seen in
 * the response of the kali-linux tracker, whereas the latter can be found in
 * the tracker response of the other two files.
*/

std::vector<Peer*> PeerRetriever::decodeResponse(std::string response) {
    // Äåêîäèðóåì îòâåò òðåêåðà
    bencode::Value decoded = bencode::Decoder::decode(response);

    // Ïðîâåðÿåì, ÷òî ýòî ñëîâàðü
    const bencode::ValueDictionary* responseDict =
        boost::get<bencode::ValueDictionary>(&decoded);
    if (!responseDict) {
        throw std::runtime_error("Invalid tracker response: expected dictionary");
    }

    // Èùåì ïîëå "peers"
    auto peersIt = responseDict->find("peers");
    if (peersIt == responseDict->end()) {
        throw std::runtime_error("Missing 'peers' field in tracker response");
    }

    std::vector<Peer*> peers;

    // Îáðàáîòêà êîìïàêòíîãî ôîðìàòà (áèíàðíàÿ ñòðîêà)
    if (const std::string* peersStr = boost::get<std::string>(&peersIt->second)) {
        const int peerInfoSize = 6;

        if (peersStr->size() % peerInfoSize != 0) {
            throw std::runtime_error("Invalid compact peers format");
        }

        for (size_t i = 0; i < peersStr->size(); i += peerInfoSize) {
            std::string ip = std::to_string((uint8_t)(*peersStr)[i]) + "." +
                std::to_string((uint8_t)(*peersStr)[i + 1]) + "." +
                std::to_string((uint8_t)(*peersStr)[i + 2]) + "." +
                std::to_string((uint8_t)(*peersStr)[i + 3]);

            int port = (uint8_t)(*peersStr)[i + 4] << 8 | (uint8_t)(*peersStr)[i + 5];
            peers.push_back(new Peer{ ip, port });
        }
    }
    // Îáðàáîòêà íåêîìïàêòíîãî ôîðìàòà (ñïèñîê ñëîâàðåé)
    else if (const bencode::ValueVector* peerList = boost::get<bencode::ValueVector>(&peersIt->second)) {
        for (const auto& item : *peerList) {
            if (const bencode::ValueDictionary* peerDict =
                boost::get<bencode::ValueDictionary>(&item)) {
                std::string ip;
                int port = 0;

                auto ipIt = peerDict->find("ip");
                if (ipIt != peerDict->end()) {
                    if (const std::string* s = boost::get<std::string>(&ipIt->second)) {
                        ip = *s;
                    }
                }

                auto portIt = peerDict->find("port");
                if (portIt != peerDict->end()) {
                    if (const unsigned long long* p = boost::get<unsigned long long>(&portIt->second)) {
                        port = static_cast<int>(*p);
                    }
                    else if (const int* p = boost::get<int>(&portIt->second)) { // âìåñòî int64_t
                        port = *p;
                    }
                }

                if (!ip.empty() && port > 0) {
                    peers.push_back(new Peer{ ip, port });
                }
            }
        }
    }
    else {
        throw std::runtime_error("Invalid 'peers' format in tracker response");
    }

    return peers;
}