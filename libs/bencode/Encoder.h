/**
 * @file Encoder.h
 * @brief Encoder for BItem hierarchy to bencoded format.
 * @details Implements conversion of BItem objects to bencoded strings.
 */
#ifndef BENCODING_ENCODER_H
#define BENCODING_ENCODER_H

#include <memory>
#include <string>
#include "BItemVisitor.h"

namespace bencoding {

    class BItem;

    /**
     * @class Encoder
     * @brief Bencoded data encoder.
     *
     * Example usage:
     * @code
     * auto encoder = Encoder::create();
     * auto str = encoder->encode(BInteger::create(42)); // returns "i42e"
     * @endcode
     */
    class Encoder : private BItemVisitor {
    public:
        /**
         * @brief Creates a new encoder
         * @return Unique pointer to new encoder
         */
        static std::unique_ptr<Encoder> create();

        /**
         * @brief Encodes BItem to bencoded string
         * @param data Item to encode
         * @return Bencoded string
         */
        std::string encode(std::shared_ptr<BItem> data);

    private:
        Encoder();

        /// @name Visitor Interface
        /// @{
        virtual void visit(BDictionary* bDictionary) override;
        virtual void visit(BInteger* bInteger) override;
        virtual void visit(BList* bList) override;
        virtual void visit(BString* bString) override;
        /// @}

    private:
        std::string encodedData; ///< Accumulated output
    };

    /// @name Convenience Functions
    /// @{
    std::string encode(std::shared_ptr<BItem> data);
    /// @}

} // namespace bencoding
#endif