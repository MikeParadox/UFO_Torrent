#ifndef CONNECT_H
#define CONNECT_H

#include <cstdint>
#include <string>

int createConnection(const std::string& ip, int port);
void sendData(int sock, const std::string& data);
std::string receiveData(int sock, uint32_t bufferSize = 0);

#endif // CONNECT_H
