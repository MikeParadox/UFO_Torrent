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

        void readExpectedChar(std::istream& input, char expected_char) const;

        /// @name Integer Decoding
        /// @{
        std::unique_ptr<BDictionary> decodeDictionary(std::istream& input);
        std::unique_ptr<BDictionary> decodeDictionaryItemsIntoDictionary(
            std::istream& input);
        std::shared_ptr<BString> decodeDictionaryKey(std::istream& input);
        std::unique_ptr<BItem> decodeDictionaryValue(std::istream& input);
        /// @}

        /// @name Integer Decoding
        /// @{
        std::unique_ptr<BInteger> decodeInteger(std::istream& input) const;
        std::string readEncodedInteger(std::istream& input) const;
        std::unique_ptr<BInteger> decodeEncodedInteger(
            const std::string& encodedInteger) const;
        /// @}

        /// @name List Decoding
        /// @{
        std::unique_ptr<BList> decodeList(std::istream& input);
        std::unique_ptr<BList> decodeListItemsIntoList(std::istream& input);
        /// @}

        /// @name String Decoding
        /// @{
        std::unique_ptr<BString> decodeString(std::istream& input) const;
        std::string::size_type readStringLength(std::istream& input) const;
        std::string readStringOfGivenLength(std::istream& input,
            std::string::size_type length) const;
        /// @}

        void validateInputDoesNotContainUndecodedCharacters(std::istream& input);
    };

    /// @name Convenience Functions
    /// @{
    std::unique_ptr<BItem> decode(const std::string& data);
    std::unique_ptr<BItem> decode(std::istream& input);
    /// @}

} // namespace bencoding
#endif