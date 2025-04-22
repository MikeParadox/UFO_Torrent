/**
 * @file encode.h
 * @brief Bencode encoding utilities.
 * @details Implements encoding of Value objects to bencoded strings.
 */
#ifndef ENCODE_H
#define ENCODE_H

#include "valueTypes.h"
#include <string>
using std::string;

namespace bencode {

    /**
     * @class Encoder
     * @brief Encodes Value objects to bencoded strings.
     */
    class Encoder {
    private:
        Encoder();

    public:
        /**
         * @brief Encodes Value to bencoded string.
         * @param value Value to encode.
         * @return Bencoded string.
         */
        static string encode(const Value& value);
    };
};

#endif