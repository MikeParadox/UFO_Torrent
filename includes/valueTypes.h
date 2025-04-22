/**
 * @file valueTypes.h
 * @brief Type definitions for bencode values.
 * @details Defines Value type and related containers for bencode decoding.
 */
#ifndef VALUETYPES_H
#define VALUETYPES_H

#include <boost/variant.hpp>
#include <string>
#include <vector>
#include <map>

namespace bencode {

    /**
     * @typedef Value
     * @brief Recursive variant type for bencode values.
     */
    using Value = boost::make_recursive_variant<
        int,
        unsigned long long,
        std::string,
        std::vector<boost::recursive_variant_>,
        std::map<std::string, boost::recursive_variant_>
    >::type;

    /**
     * @typedef ValueDictionary
     * @brief Dictionary of bencode values.
     */
    using ValueDictionary = std::map<std::string, Value>;

    /**
     * @typedef ValueVector
     * @brief Vector of bencode values.
     */
    using ValueVector = std::vector<Value>;
};

#endif