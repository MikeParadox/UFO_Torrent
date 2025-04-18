#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string urlEncode(const std::string& value);

std::string hexDecode(const std::string& value);

std::string hexEncode(const std::string& input);

bool hasPiece(const std::string& bitField, int index);

void setPiece(std::string& bitField, int index);

int bytesToInt(std::string bytes);

std::string formatTime(long seconds);

#endif // UTILS_H
