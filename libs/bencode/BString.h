/**
 * @file BString.h
 * @brief Representation of a bencoded string.
 * @details Implements string type for bencoding format using std::string as storage.
 */
#ifndef BENCODING_BSTRING_H
#define BENCODING_BSTRING_H

#include <memory>
#include <string>
#include "BItem.h"

namespace bencoding {

    /**
     * @class BString
     * @brief Bencoded string implementation.
     *
     * Example usage:
     * @code
     * auto str = BString::create("hello");
     * std::string s = str->value();
     * @endcode
     */
    class BString : public BItem {
    public:
        using ValueType = std::string; ///< Underlying string type

    public:
        /**
         * @brief Creates a new string
         * @param value Initial value
         * @return Unique pointer to new string
         */
        static std::unique_ptr<BString> create(ValueType value);

        /**
         * @brief Gets current value
         * @return Current string value
         */
        ValueType value() const;

        /**
         * @brief Sets new value
         * @param value New value to set
         */
        void setValue(ValueType value);

        /**
         * @brief Gets string length
         * @return Length of string in bytes
         */
        ValueType::size_type length() const;

        /// @name Visitor Pattern
        /// @{
        virtual void accept(BItemVisitor* visitor) override;
        /// @}

    private:
        explicit BString(ValueType value);

    private:
        ValueType _value; ///< Stored string value
    };

} // namespace bencoding
#endif