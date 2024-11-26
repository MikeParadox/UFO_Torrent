#ifndef VALUETYPES_H
#define VALUETYPES_H

#include <boost/variant.hpp>

#include <string>
#include <vector>
#include <map>

namespace bencode {

    using Value = boost::make_recursive_variant<int, unsigned long long, std::string, std::vector<boost::recursive_variant_>, std::unordered_map<std::string, boost::recursive_variant_>>::type;
    using ValueDictionary = std::unordered_map<std::string, Value>;
    using ValueVector = std::vector<Value>;

};

#endif