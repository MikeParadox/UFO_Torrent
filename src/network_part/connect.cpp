#include "connect.h"
#include "iostream"
#include "utils.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <limits>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int connection_timeout{ 10 }; // ���� 3
constexpr int read_timeout{ 10000 };

// Устанавливаем сокет в блокирующий/неблокирующий режим
bool setSocketBlocking(int sock, bool blocking)
{
    if (sock < 0) return false;

    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) return false;

    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(sock, F_SETFL, flags) == 0);
}

// Создает соединение по TCP
int createConnection(const std::string& ip, const int port)
{
    int sock = 0;
    struct sockaddr_in address;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("Socket creation error: " + std::to_string(sock));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    char* tempIp{ new char[ip.length() + 1] };
    strcpy(tempIp, ip.c_str());

    // Преобразуем IP-адрес из строки
    if (inet_pton(AF_INET, tempIp, &address.sin_addr) <= 0)
        throw std::runtime_error("Invalid IP address: " + ip);

    // Ставим сокет в неблокирующий режим для connect
    if (!setSocketBlocking(sock, false))
        throw std::runtime_error("An error occurred when setting socket "
            + std::to_string(sock) + " to NONBLOCK");

    connect(sock, (struct sockaddr*)&address, sizeof(address));

    fd_set fdset;
    struct timeval tv;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = connection_timeout;
    tv.tv_usec = 0;

    // Ожидаем, пока connect завершится
    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len{ sizeof so_error };
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0)
        {
            // Переключаемся обратно в блокирующий режим
            if (!setSocketBlocking(sock, true))
                throw std::runtime_error("An error occurred when setting socket "
                    + std::to_string(sock) + " to BLOCK");
            return sock;
        }
    }

    close(sock);
    throw std::runtime_error("Connect to " + ip + ":" + std::to_string(port)
        + ": FAILED [Connection timeout]");
}

// Отправка данных пиру
void sendData(const int sock, const std::string& data)
{
    auto n = data.length();
    if (n == 0) return;
    int res = send(sock, data.data(), n, 0);
    if (res < 0)
        throw std::runtime_error("Failed to write data to socket "
            + std::to_string(sock));
}

// Корректное чтение данных BitTorrent-сообщений
std::string receiveData(int sock, int expected_size)
{
    if (sock <= 0) {
        throw std::runtime_error("Invalid socket");
    }

    // Если размер не задан явно, значит сначала берем префикс длины
    if (expected_size == 0) {
        uint32_t length_prefix = 0;
        ssize_t bytes_received = recv(sock, &length_prefix, 4, 0);

        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                throw std::runtime_error("Connection closed by peer");
            }
            else {
                throw std::runtime_error("Failed to receive data: "
                    + std::string(strerror(errno)));
            }
        }

        if (bytes_received < 4) {
            throw std::runtime_error("Incomplete length prefix received");
        }

        length_prefix = ntohl(length_prefix); // big-endian -> host order

        // Если ноль, то это keep-alive
        if (length_prefix == 0) {
            return "";
        }
        expected_size = length_prefix;
    }

    std::vector<char> buffer(expected_size);
    ssize_t total_bytes = 0;

    // Считываем весь ожидаемый объем данных по частям
    while (total_bytes < expected_size) {
        ssize_t bytes = recv(sock, buffer.data() + total_bytes,
            expected_size - total_bytes, 0);
        if (bytes <= 0) {
            if (bytes == 0) {
                throw std::runtime_error("Connection closed by peer during message receive");
            }
            else {
                throw std::runtime_error("Error receiving data: " + std::string(strerror(errno)));
            }
        }
        total_bytes += bytes;
    }

    // Превращаем полученный буфер в std::string
    return std::string(buffer.data(), buffer.size());
}