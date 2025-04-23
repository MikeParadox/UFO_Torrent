/**
 * @file decode.h
 * @brief Bencode decoding utilities.
 * @details Implements decoding of bencoded torrent data.
 */
#ifndef DECODE_H
#define DECODE_H

#include "valueTypes.h"

namespace bencode
{
    /**
     * @class Decoder
     * @brief Decodes bencoded strings into Value objects.
     */
    class Decoder
    {
    public:
        friend struct Dt;

        /**
         * @brief Decodes bencoded string.
         * @param string Bencoded input string.
         * @return Decoded Value object.
         */
        static Value decode(const std::string& string);

    private:
        static std::string firstDigit(const std::string& str);
        static std::pair<unsigned long long, int> decodeInt(const std::string& s);
        static std::pair<std::string, int> decodestring(const std::string& s);
        static std::pair<ValueVector, int> decodeList(const std::string& s);
        static std::pair<ValueDictionary, int> decodeDict(const std::string& s);
        static std::pair<Value, int> _decode(const std::string& s);
    };

    /**
     * @struct Dt
     * @brief Helper for dictionary decoding.
     */
    struct Dt
    {
        std::pair<ValueDictionary, int> decodeDc(const std::string& s);
        Decoder decoder;
    };
}

#endif