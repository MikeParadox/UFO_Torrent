#include "./decode.h"
#include <boost/variant.hpp>
#include <tuple>

using namespace bencode;
using std::string;
using std::pair;
using std::string;

string Decoder::firstDigit(const string& str) {
    string res = "";

    // TODO : issue #12

    return res;
}

pair<int, int> Decoder::decodeInt(const string& s) {

    size_t eIndex = s.find_first_of('e');
    int i = stoi(s.substr(1, eIndex));

    return pair<int, int>(i, eIndex + 1);
}

pair<string, int> Decoder::decodeString(const string& s) {

    string lengthPart = firstDigit(s);
    size_t digitsInString = lengthPart.size();

    int length = stoi(lengthPart);

    return pair<string, int>(s.substr(digitsInString + 1, length), digitsInString + 1 + length);
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

    pair<int, int> pairInt;
    pair<string, int> pairString;

    while (0 < s.length()) {

        char currChar = s[index];

        switch (currChar)
        {

        case 'i':
            pairInt = decodeInt(s.substr(index));
            index += pairInt.second;
            result = pair<Value, int>(pairInt.first, index);
            return result;

        case 'l':
            pairList = decodeList(s.substr(index));
            index += pairList.second;
            result = pair<ValueVector, int>(pairList.first, index);
            return result;

        case 'd':
            pairDict = decodeDict(s.substr(index));
            index += pairDict.second;
            result = pair<ValueDictionary, int>(pairDict.first, index);
            return result;

        default:
            pairString = decodeString(s.substr(index));
            index += pairString.second;
            result = pair<Value, int>(pairString.first, index);
            return result;
        }
    }

    return result;
}

Value Decoder::decode(const string& string)
{
    return _decode(string).first;
}