/**
 * @file BDictionary.h
 * @brief Representation of a bencoded dictionary (key-value pairs).
 * @details Implements a dictionary structure that maintains keys in lexicographical order
 *          as required by the BitTorrent specification. The interface mimics std::map.
 * @see https://wiki.theory.org/BitTorrentSpecification#Bencoding
 */
#ifndef BENCODING_BDICTIONARY_H
#define BENCODING_BDICTIONARY_H

#include <initializer_list>
#include <map>
#include <memory>
#include "BItem.h"

namespace bencoding {

    class BString;

    /**
     * @class BDictionary
     * @brief Bencoded dictionary implementation.
     *
     * Example usage:
     * @code
     * auto dict = BDictionary::create();
     * (*dict)[BString::create("key")] = BString::create("value");
     * @endcode
     */
    class BDictionary : public BItem {
    private:
        /**
         * @class BStringByValueComparator
         * @brief Comparator for BString smart pointers.
         * @details Compares the string values rather than pointer addresses.
         */
        class BStringByValueComparator {
        public:
            /**
             * @brief Comparison operator
             * @param lhs Left-hand side string
             * @param rhs Right-hand side string
             * @return true if lhs < rhs by value
             */
            bool operator()(const std::shared_ptr<BString>& lhs,
                const std::shared_ptr<BString>& rhs) const;
        };

        /// Map type using custom comparator
        using BItemMap = std::map<std::shared_ptr<BString>,
            std::shared_ptr<BItem>, BStringByValueComparator>;

    public:
        // Standard map typedefs
        using key_type = BItemMap::key_type;
        using mapped_type = BItemMap::mapped_type;
        using value_type = BItemMap::value_type;
        using size_type = BItemMap::size_type;
        using reference = BItemMap::reference;
        using const_reference = BItemMap::const_reference;
        using iterator = BItemMap::iterator;
        using const_iterator = BItemMap::const_iterator;

    public:
        /**
         * @brief Creates an empty dictionary
         * @return Unique pointer to new dictionary
         */
        static std::unique_ptr<BDictionary> create();

        /**
         * @brief Creates a dictionary from initializer list
         * @param items Initial key-value pairs
         * @return Unique pointer to new dictionary
         */
        static std::unique_ptr<BDictionary> create(
            std::initializer_list<value_type> items);

        /// @name Capacity
        /// @{
        size_type size() const;
        bool empty() const;
        /// @}

        /// @name Element Access
        /// @{
        /**
         * @brief Accesses or creates element by key
         * @param key Dictionary key
         * @return Reference to mapped value
         */
        mapped_type& operator[](const key_type& key);
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

        /**
         * @brief Gets value by string key
         * @param key String key to lookup
         * @return Shared pointer to item or nullptr if not found
         */
        std::shared_ptr<BItem> getValue(const std::string& key);

    private:
        BDictionary();
        explicit BDictionary(std::initializer_list<value_type> items);

    private:
        BItemMap itemMap; ///< Underlying map storage
    };

} // namespace bencoding
#endif