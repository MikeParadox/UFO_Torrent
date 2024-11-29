#include "../includes/decode.h"
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <future>
#include <tuple>

using namespace bencode;
using std::string;
using std::pair;


bool stringToLongLong(const std::string& str, unsigned long long& result) {
    try {
        result = boost::lexical_cast<unsigned long long>(str);
        return true;
    }
    catch (const boost::bad_lexical_cast&) {
        return false;
    }
}

string Decoder::firstDigit(const string& str) {
    string number;
    for (int i = 0; i < str.size(); ++i) {
        if (isdigit(str[i]))
            number += str[i];
        else if (!number.empty())
            break;
    }
    return number;
}

pair<unsigned long long, int> Decoder::decodeInt(const string& s) {
    size_t eIndex = s.find_first_of('e');
    
    unsigned long long value;
    if (stringToLongLong(s.substr(1, eIndex - 1), value))
        return pair<unsigned long long, int>(value, eIndex + 1);
    else
        throw std::invalid_argument("Wrong int in file");
}

pair<string, int> Decoder::decodestring(const string& s) {
    if (!isdigit(s[0])) 
        throw std::invalid_argument("Wrong file struct");

    string lengthPart = firstDigit(s);
    size_t digitsInstring = lengthPart.size();

    int length = stoi(lengthPart);

    if (digitsInstring + 1 + length > s.size() || s[digitsInstring + 1] == ':')
        throw std::invalid_argument("Wrong file struct");

    return pair<string, int>(
        s.substr(digitsInstring + 1, length),
        digitsInstring + 1 + length
    );
}

pair<ValueVector, int> Decoder::decodeList(const string& s) {
    ValueVector xs; 
    int index = 1; 

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') {
            index += 1;
            break;
        }

        int i = 0;
        Value b;

        std::tie(b, i) = (_decode(s.substr(index)));

        xs.push_back(b); 
        index += i;    
    }

    return pair<ValueVector, int>(xs, index);
}

pair<ValueDictionary, int> Decoder::decodeDict(const string& s) {
    ValueDictionary dict; 
    int index = 1; 

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') { 
            index += 1;
            break;
        }

        Value key;
        int i1;
        Value value;
        int i2;


        std::tie(key, i1) = _decode(s.substr(index));
        std::tie(value, i2) = _decode(s.substr(index + i1));

        dict[boost::get<string>(key)] = value;
        index += i1 + i2;
    }

    return pair<ValueDictionary, int>(dict, index); 
}

pair<Value, int> Decoder::_decode(const string& s) {
    int index = 0;

    pair<Value, int> result;
    pair<ValueVector, int> pairList;
    pair<ValueDictionary, int> pairDict;
    pair<unsigned long long, int> pairInt;
    pair<string, int> pairstring;

    while (0 < s.length()) {
        char currChar = s[index];

        switch (currChar) {
        case 'i': 
            pairInt = decodeInt(s.substr(index));
            index += pairInt.second;  
            result = pair<Value, int>(pairInt.first, index);
            return result;

        case 'l':
            pairList = decodeList(s.substr(index));
            index += pairList.second;
            result = pair<Value, int>(pairList.first, index);
            return result;

        case 'd':
            pairDict = decodeDict(s.substr(index));
            index += pairDict.second;
            result = pair<Value, int>(pairDict.first, index);
            return result;

        default:
            pairstring = decodestring(s.substr(index));
            index += pairstring.second;
            result = pair<Value, int>(pairstring.first, index);
            return result;
        }
    }

    return result;
}

Value Decoder::decode(const string& string) {
    return _decode(string).first;
}

pair<ValueDictionary, int> Dt::decodeDc(const string& s) { return Decoder::decodeDict(s); }
