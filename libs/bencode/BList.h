/**
 * @file BList.h
 * @brief Representation of a bencoded list (sequence of items).
 * @details Implements a list structure for bencoded data with interface similar to std::list.
 */
#if BENCODING_BLIST_H
#define BENCODING_BLIST_H

#include <initializer_list>
#include <list>
#include <memory>
#include "BItem.h"

namespace bencoding {

    /**
     * @class BList
     * @brief Bencoded list implementation.
     *
     * Example usage:
     * @code
     * auto list = BList::create();
     * list->push_back(BInteger::create(42));
     * list->push_back(BString::create("test"));
     * @endcode
     */
    class BList : public BItem {
    private:
        using BItemList = std::list<std::shared_ptr<BItem>>;

    public:
        // Standard list typedefs
        using value_type = BItemList::value_type;
        using size_type = BItemList::size_type;
        using reference = BItemList::reference;
        using const_reference = BItemList::const_reference;
        using iterator = BItemList::iterator;
        using const_iterator = BItemList::const_iterator;

    public:
        /**
         * @brief Creates an empty list
         * @return Unique pointer to new list
         */
        static std::unique_ptr<BList> create();

        /**
         * @brief Creates a list from initializer list
         * @param items Initial items
         * @return Unique pointer to new list
         */
        static std::unique_ptr<BList> create(std::initializer_list<value_type> items);

        /// @name Capacity
        /// @{
        size_type size() const;
        bool empty() const;
        /// @}

        /// @name Modifiers
        /// @{
        void push_back(const value_type& bItem);
        void pop_back();
        /// @}

        /// @name Element Access
        /// @{
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        /// @}

        /// @name Iterators
        /// @{
        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;
        const_iterator cbegin() const;
        const_iterator cend() const;
        /// @}

        /// @name Visitor Pattern
        /// @{
        virtual void accept(BItemVisitor* visitor) override;
        /// @}

    private:
        BList();
        explicit BList(std::initializer_list<value_type> items);

    private:
        BItemList itemList; ///< Underlying list storage
    };

} // namespace bencoding
#endif