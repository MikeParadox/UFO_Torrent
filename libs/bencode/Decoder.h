/**
 * @file Decoder.h
 * @brief Decoder for bencoded data.
 * @details Implements parsing of bencoded data into BItem hierarchy.
 * @throws DecodingError on invalid input
 */
#ifndef BENCODING_DECODER_H
#define BENCODING_DECODER_H

#include <exception>
#include <memory>
#include <string>
#include "BItem.h"

namespace bencoding {

    class BDictionary;
    class BInteger;
    class BList;
    class BString;

    /**
     * @class DecodingError
     * @brief Exception thrown when decoding fails.
     */
    class DecodingError : public std::runtime_error {
    public:
        explicit DecodingError(const std::string& what);
    };

    /**
     * @class Decoder
     * @brief Bencoded data decoder.
     *
     * Example usage:
     * @code
     * auto decoder = Decoder::create();
     * auto item = decoder->decode("i42e");
     * @endcode
     */
    class Decoder {
    public:
        /**
         * @brief Creates a new decoder
         * @return Unique pointer to new decoder
         */
        static std::unique_ptr<Decoder> create();

        /**
         * @brief Decodes bencoded string
         * @param data Input string
         * @return Decoded item
         * @throws DecodingError on invalid input
         */
        std::unique_ptr<BItem> decode(const std::string& data);

        /**
         * @brief Decodes from input stream
         * @param input Input stream
         * @return Decoded item
         * @throws DecodingError on invalid input
         */
        std::unique_ptr<BItem> decode(std::istream& input);

    private:
        Decoder();

        // Decoding helper methods...
    };

    /// @name Convenience Functions
    /// @{
    std::unique_ptr<BItem> decode(const std::string& data);
    std::unique_ptr<BItem> decode(std::istream& input);
    /// @}

} // namespace bencoding
#endif