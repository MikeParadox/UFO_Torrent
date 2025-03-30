#include "../includes/decode.h"
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <future>
#include <tuple>

using namespace bencode;
using std::string;
using std::pair;

bool stringToUnsignedLongLong(const std::string& str, unsigned long long& result) {
    try {
        result = boost::lexical_cast<unsigned long long>(str);
        return true;
    }
    catch (const boost::bad_lexical_cast&) {
        return false;
    }
}

std::string Decoder::firstDigit(const std::string& str) {
    std::string number;
    for (const char ch : str) {
        if (std::isdigit(static_cast<unsigned char>(ch)))
            number.push_back(ch);
        else if (!number.empty())
            break;
    }
    return number;
}

std::pair<unsigned long long, int> Decoder::decodeInt(const std::string& s) {
    auto eIndex = s.find_first_of('e');
    if (eIndex == std::string::npos)
        throw std::invalid_argument("Invalid integer encoding: missing 'e'");

    std::string intPart = s.substr(1, eIndex - 1);
    unsigned long long value;
    if (!stringToUnsignedLongLong(intPart, value))
        throw std::invalid_argument("Invalid integer value: " + intPart);

    return { value, static_cast<int>(eIndex + 1) };
}

std::pair<std::string, int> Decoder::decodestring(const std::string& s) {
    if (s.empty() || !std::isdigit(static_cast<unsigned char>(s[0])))
        throw std::invalid_argument("Invalid string encoding: missing length");

    std::string lengthPart = firstDigit(s);
    int digitsInString = static_cast<int>(lengthPart.size());
    int length = std::stoi(lengthPart);

    if (s.size() < static_cast<size_t>(digitsInString + 1) || s[digitsInString] != ':')
        throw std::invalid_argument("Invalid string encoding: missing colon");

    if (s.size() < static_cast<size_t>(digitsInString + 1 + length))
        throw std::invalid_argument("Invalid string encoding: length exceeds available data");

    return { s.substr(digitsInString + 1, length), digitsInString + 1 + length };
}

std::pair<ValueVector, int> Decoder::decodeList(const std::string& s) {
    ValueVector xs;
    int index = 1;

    while (index < static_cast<int>(s.length())) {
        if (s[index] == 'e') {
            ++index;
            break;
        }
        auto [value, offset] = _decode(s.substr(index));
        xs.push_back(value);
        index += offset;
    }

    return { xs, index };
}

std::pair<ValueDictionary, int> Decoder::decodeDict(const std::string& s) {
    ValueDictionary dict;
    int index = 1;

    while (index < static_cast<int>(s.length())) {
        if (s[index] == 'e') {
            ++index; 
            break;
        }
        auto [keyValue, keyOffset] = _decode(s.substr(index));

        const std::string* keyStr = boost::get<std::string>(&keyValue);
        if (!keyStr)
            throw std::invalid_argument("Invalid dictionary key: key must be a string");

        index += keyOffset;
        auto [value, valueOffset] = _decode(s.substr(index));
        dict[*keyStr] = value;
        index += valueOffset;
    }

    return { dict, index };
}

std::pair<Value, int> Decoder::_decode(const std::string& s) {
    if (s.empty())
        throw std::invalid_argument("Cannot decode empty string");

    int index = 0;
    char currChar = s[index];

    switch (currChar) {
    case 'i': {
        auto [value, offset] = decodeInt(s);
        return { value, offset };
    }
    case 'l': {
        auto [list, offset] = decodeList(s);
        return { list, offset };
    }
    case 'd': {
        auto [dict, offset] = decodeDict(s);
        return { dict, offset };
    }
    default: {
        auto [strValue, offset] = decodestring(s);
        return { strValue, offset };
    }
    }
}

Value Decoder::decode(const std::string& str) {
    return _decode(str).first;
}

std::pair<ValueDictionary, int> Dt::decodeDc(const std::string& s) {
    return Decoder::decodeDict(s);
}

