/**
 * @file BInteger.h
 * @brief Representation of a bencoded integer.
 * @details Implements integer type for bencoding format with 64-bit range.
 */
#ifndef BENCODING_BINTEGER_H
#define BENCODING_BINTEGER_H

#include <cstdint>
#include <memory>
#include "BItem.h"

namespace bencoding {

    /**
     * @class BInteger
     * @brief Bencoded integer value.
     *
     * Example usage:
     * @code
     * auto num = BInteger::create(42);
     * int64_t val = num->value();
     * @endcode
     */
    class BInteger : public BItem {
    public:
        using ValueType = int64_t; ///< Underlying integer type

    public:
        /**
         * @brief Creates a new integer
         * @param value Initial value
         * @return Unique pointer to new integer
         */
        static std::unique_ptr<BInteger> create(ValueType value);

        /**
         * @brief Gets current value
         * @return Current integer value
         */
        ValueType value() const;

        /**
         * @brief Sets new value
         * @param value New value to set
         */
        void setValue(ValueType value);

        /// @name Visitor Pattern
        /// @{
        virtual void accept(BItemVisitor* visitor) override;
        /// @}

    private:
        explicit BInteger(ValueType value);

    private:
        ValueType _value; ///< Stored integer value
    };

} // namespace bencoding
#endif