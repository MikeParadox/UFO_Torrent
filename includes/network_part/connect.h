/**
 * @file connect.h
 * @brief Network connection utilities for peer communication.
 * @details Provides functions for creating connections and data transfer.
 */
#ifndef CONNECT_H
#define CONNECT_H

#include <cstdint>
#include <string>

 /**
  * @brief Creates a TCP connection to a peer.
  * @param ip Peer IP address.
  * @param port Peer port number.
  * @return Socket descriptor on success, -1 on error.
  */
int createConnection(const std::string& ip, int port);

/**
 * @brief Sends data through a socket.
 * @param sock Socket descriptor.
 * @param data Data to send.
 */
void sendData(int sock, const std::string& data);

/**
 * @brief Receives data from a socket.
 * @param sock Socket descriptor.
 * @param bufferSize Buffer size for receiving (0 for default).
 * @return Received data as string.
 */
std::string receiveData(int sock, int bufferSize = 0);

#endif // CONNECT_H