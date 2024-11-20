#include "../includes/decode.h"
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <tuple>

using namespace bencode;
using std::string;
using std::pair;
using std::string;



bool stringToLongLong(const std::string& str, long long& result) {
    try {
        result = boost::lexical_cast<long long>(str);
        return true;
    }
    catch (const boost::bad_lexical_cast&) {
        return false;
    }
}

// ������� ��� ��������� ������ ������������������ ���� �� ������.
string Decoder::firstDigit(const string& str) {
    std::string number;
    for (int i = 0; i < str.size(); ++i) {
        if (isdigit(str[i]))
            number += str[i];
        else if (!number.empty())
            break;
    }
    return number; // ���������� ��������� ������������������ ����
}

// ������������� ������ ����� � ������� Bencode.
// ������: "i<�����>e" (��������, "i42e" -> 42).
pair<long long, int> Decoder::decodeInt(const string& s) {
    size_t eIndex = s.find_first_of('e'); // ������� ������� 'e', ������������ �����
    
    long long value;
    if (stringToLongLong(s.substr(1, eIndex - 1), value))
        return pair<long long, int>(value, eIndex + 1);
    else
        throw std::invalid_argument("Wrong int in file");
}

// ������������� ������ � ������� Bencode.
// ������: "<�����>:<������>" (��������, "4:spam" -> "spam").
pair<string, int> Decoder::decodeString(const string& s) {
    string lengthPart = firstDigit(s);
    size_t digitsInString = lengthPart.size();

    int length = stoi(lengthPart);

    return pair<string, int>(
        s.substr(digitsInString + 1, length),
        digitsInString + 1 + length
    );
}

// ������������� ������ � ������� Bencode.
// ������: "l<��������>e" (��������, "l4:spami42ee" -> ["spam", 42]).
pair<ValueVector, int> Decoder::decodeList(const string& s) {
    ValueVector xs; // ������ ��� �������� �������������� ��������� ������
    int index = 1;  // �������� ����� 'l'

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') { // ���� �������� 'e', ��������� ������
            index += 1;
            break;
        }

        int i = 0;
        Value b;
        // ���������� ���������� ������� ������
        std::tie(b, i) = (_decode(s.substr(index)));

        xs.push_back(b); // ��������� �������������� ������� � ������
        index += i;      // �������� ������ �� ����� ������������� ��������
    }

    return pair<ValueVector, int>(xs, index); // ���������� ������ � ����� ������������ ������
}

// ������������� ������� � ������� Bencode.
// ������: "d<����-��������>e" (��������, "d3:cat4:spami42ee" -> {"cat": "spam", "age": 42}).
pair<ValueDictionary, int> Decoder::decodeDict(const string& s) {
    ValueDictionary dict; // ������� ��� �������� �������������� ��� ����-��������
    int index = 1;        // �������� ����� 'd'

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') { // ���� �������� 'e', ��������� �������
            index += 1;
            break;
        }

        Value key;
        int i1;
        Value value;
        int i2;

        // ���������� ���� (������)
        std::tie(key, i1) = _decode(s.substr(index));
        // ���������� ��������
        std::tie(value, i2) = _decode(s.substr(index + i1));

        // ��������� ���� ����-�������� � �������
        dict[boost::get<string>(key)] = value;
        index += i1 + i2; // �������� ������ �� ����� ������������ ������
    }

    return pair<ValueDictionary, int>(dict, index); // ���������� ������� � ����� ������������ ������
}

// �������� ��������������� �������, ������� ����������, ��� ������������ ��������
// (�����, ������, ������ ��� �������).
pair<Value, int> Decoder::_decode(const string& s) {
    int index = 0; // ������� ������

    // ���� ��� �������� ������������� �����������
    pair<Value, int> result;
    pair<ValueVector, int> pairList;
    pair<ValueDictionary, int> pairDict;
    pair<long long, int> pairInt;
    pair<string, int> pairString;

    while (0 < s.length()) {
        char currChar = s[index];

        // ���������� ��� �������� �� ������� �������
        switch (currChar) {
        case 'i': // ����� �����
            pairInt = decodeInt(s.substr(index)); // ���������� �����
            index += pairInt.second;             // ��������� ������
            result = pair<Value, int>(pairInt.first, index); // ����������� ���������
            return result;

        case 'l': // ������
            pairList = decodeList(s.substr(index)); // ���������� ������
            index += pairList.second;             // ��������� ������
            result = pair<Value, int>(pairList.first, index); // ����������� ���������
            return result;

        case 'd': // �������
            pairDict = decodeDict(s.substr(index)); // ���������� �������
            index += pairDict.second;             // ��������� ������
            result = pair<Value, int>(pairDict.first, index); // ����������� ���������
            return result;

        default: // ������
            pairString = decodeString(s.substr(index)); // ���������� ������
            index += pairString.second;                // ��������� ������
            result = pair<Value, int>(pairString.first, index); // ����������� ���������
            return result;
        }
    }

    return result; // ���������� ��������� (�������� � ����� ������������ ������)
}

// �������� ����� �������������: �������� _decode � ���������� �������������� ��������.
Value Decoder::decode(const string& string) {
    return _decode(string).first; // ���������� ������ ��������, ��������� �����
}


std::pair<std::string, int> Str::decodeStr(const std::string& s) { return Decoder::decodeString(s); }
