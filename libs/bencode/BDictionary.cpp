/**
* @file      BDictionary.cpp
* @copyright (c) 2014 by Petr Zemek (s3rvac@gmail.com) and contributors
* @license   BSD, see the @c LICENSE file for more details
* @brief     Implementation of the BDictionary class.
*/

#include "BDictionary.h"

#include <cassert>
#include <iostream>

#include "BItemVisitor.h"
#include "BString.h"
#include "BList.h"

namespace bencoding {

/**
 * @brief Comparator for BString objects based on their string values
 * @param lhs Left-hand side BString to compare
 * @param rhs Right-hand side BString to compare
 * @return true if lhs's string value is less than rhs's string value
 * 
 * @details Enforces lexicographical ordering of dictionary keys as required by
 *          the BitTorrent specification. Compares the actual string contents
 *          rather than pointer addresses.
 */
bool BDictionary::BStringByValueComparator::operator()(
		const std::shared_ptr<BString> &lhs,
		const std::shared_ptr<BString> &rhs) const {
	return lhs->value() < rhs->value();
}

/// @name Constructors/Destructors
/// @{

/**
 * @brief Constructs an empty BDictionary
 */
BDictionary::BDictionary() = default;

/**
 * @brief Constructs a BDictionary with initial key-value pairs
 * @param items Initializer list of key-value pairs to populate the dictionary
 */
BDictionary::BDictionary(std::initializer_list<value_type> items):
	itemMap(items) {}

/// @}

/// @name Factory Methods
/// @{

/**
 * @brief Creates a new empty BDictionary instance
 * @return std::unique_ptr to the newly created dictionary
 */
std::unique_ptr<BDictionary> BDictionary::create() {
	return std::unique_ptr<BDictionary>(new BDictionary());
}

/// @}

/// @name Factory Methods
/// @{

/**
 * @brief Creates a new empty BDictionary instance
 * @return std::unique_ptr to the newly created dictionary
 */
std::unique_ptr<BDictionary> BDictionary::create(
		std::initializer_list<value_type> items) {
	return std::unique_ptr<BDictionary>(new BDictionary(items));
}

/// @}

/// @name Capacity
/// @{

/**
 * @brief Returns the number of elements in the dictionary
 * @return Number of key-value pairs in the dictionary
 */
BDictionary::size_type BDictionary::size() const {
	return itemMap.size();
}

/**
 * @brief Checks whether the dictionary is empty
 * @return true if the dictionary contains no elements, false otherwise
 */
bool BDictionary::empty() const {
	return itemMap.empty();
}

/// @}

/// @name Element Access
/// @{

/**
 * @brief Accesses specified element with bounds checking
 * @param key Key of the element to find
 * @return Reference to the mapped value of the element
 * 
 * @note If the key doesn't exist, inserts a new element with that key and
 *       a nullptr value (similar to std::map behavior)
 */
BDictionary::mapped_type &BDictionary::operator[](const key_type &key) {
	return itemMap[key];
}

/// @}

/// @name Iterators
/// @{

/**
 * @brief Returns an iterator to the beginning of the dictionary
 * @return Iterator pointing to the first element
 * 
 * @note Elements are ordered lexicographically by their string keys
 */
BDictionary::iterator BDictionary::begin() {
	return itemMap.begin();
}

/**
 * @brief Returns an iterator to the end of the dictionary
 * @return Iterator pointing to the past-the-end element
 */
BDictionary::iterator BDictionary::end() {
	return itemMap.end();
}

/**
* @brief Returns a constant iterator to the beginning of the dictionary.
*/
BDictionary::const_iterator BDictionary::begin() const {
	return itemMap.begin();
}

/**
* @brief Returns a constant iterator to the end of the dictionary.
*/
BDictionary::const_iterator BDictionary::end() const {
	return itemMap.end();
}

/**
* @brief Returns a constant iterator to the beginning of the dictionary.
*/
BDictionary::const_iterator BDictionary::cbegin() const {
	return itemMap.cbegin();
}

/**
* @brief Returns a constant iterator to the end of the dictionary.
*/
BDictionary::const_iterator BDictionary::cend() const {
	return itemMap.cend();
}

// (Similar Doxygen comments for const_iterator versions)
/// @}

/**
 * @brief Accepts a visitor (Visitor pattern implementation)
 * @param visitor Pointer to the visitor object
 * 
 * @see BItemVisitor
 */
void BDictionary::accept(BItemVisitor *visitor) {
	visitor->visit(this);
}

/**
 * @brief Recursively searches for a value by key in the dictionary hierarchy
 * @param key The string key to search for
 * @return std::shared_ptr to the found BItem or nullptr if not found
 * 
 * @details Performs a depth-first search through nested dictionaries and lists.
 * The search order is:
 * 1. Current dictionary level
 * 2. Any nested BDictionary values
 * 3. Any BList values that may contain dictionaries
 * 
 * @complexity O(n) where n is total number of dictionary entries in hierarchy
 */
std::shared_ptr<BItem> BDictionary::getValue(const std::string& key)
{
    for (const auto& item : *this)
    {
        if (item.first->value() == key)
        {
            return item.second;
        }

        // Checks if the value which the key maps to is a BDictionary
        if (typeid(*item.second) == typeid(BDictionary))
        {
            std::shared_ptr<BDictionary> subDictionary = std::dynamic_pointer_cast<BDictionary>(item.second);
            auto potentialValue = subDictionary->getValue(key);
            if (potentialValue)
                return potentialValue;
        }
        // Checks if the value which the key maps to is a BList
        else if (typeid(*item.second) == typeid(BList))
        {
            std::shared_ptr<BList> subList = std::dynamic_pointer_cast<BList>(item.second);
            auto potentialValue = subList->getValue(key);
            if (potentialValue)
                return potentialValue;
        }
    }
    return std::shared_ptr<BItem>();
}

} // namespace bencoding
