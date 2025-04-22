/**
 * @file BItem.h
 * @brief Base class for all bencoded items.
 * @details Provides common interface and visitor pattern support for all bencoded types.
 */
#ifndef BENCODING_BITEM_H
#define BENCODING_BITEM_H

#include <memory>

namespace bencoding {

    class BItemVisitor;

    /**
     * @class BItem
     * @brief Abstract base class for all bencoded items.
     *
     * This class implements:
     * - Visitor pattern support
     * - Safe downcasting via as<T>()
     * - Shared pointer management
     */
    class BItem : public std::enable_shared_from_this<BItem> {
    public:
        virtual ~BItem() = 0;

        /// @name Visitor Pattern
        /// @{
        /**
         * @brief Accepts a visitor
         * @param visitor Visitor to accept
         * @note Implementations should call visitor->visit(this)
         */
        virtual void accept(BItemVisitor* visitor) = 0;
        /// @}

        /**
         * @brief Safe downcast to derived type
         * @tparam T Target type (must inherit from BItem)
         * @return Shared pointer to casted object or nullptr if cast fails
         *
         * Example:
         * @code
         * auto item = ...;
         * if (auto str = item->as<BString>()) {
         *     // use str as BString
         * }
         * @endcode
         */
        template <typename T>
        std::shared_ptr<T> as() {
            static_assert(std::is_base_of<BItem, T>::value,
                "T has to be a subclass of BItem");
            return std::dynamic_pointer_cast<T>(shared_from_this());
        }

    protected:
        BItem();

    private:
        BItem(const BItem&) = delete;
        BItem& operator=(const BItem&) = delete;
    };

} // namespace bencoding
#endif