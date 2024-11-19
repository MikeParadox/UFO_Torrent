#include "decode.h"
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

// Функция для получения первой последовательности цифр из строки.
string Decoder::firstDigit(const string& str) {
    std::string number;
    for (int i = 0; i < str.size(); ++i) {
        if (isdigit(str[i]))
            number += str[i];
        else if (!number.empty())
            break;
    }
    return number; // Возвращаем найденную последовательность цифр
}

// Декодирование целого числа в формате Bencode.
// Формат: "i<число>e" (например, "i42e" -> 42).
pair<long long, int> Decoder::decodeInt(const string& s) {
    size_t eIndex = s.find_first_of('e'); // Находим позицию 'e', завершающего число

    long long value;
    if (stringToLongLong(s.substr(1, eIndex - 1), value))
        return pair<long long, int>(value, eIndex + 1);
    else
        throw std::invalid_argument("Wrong int in file");
}

// Декодирование строки в формате Bencode.
// Формат: "<длина>:<строка>" (например, "4:spam" -> "spam").
pair<string, int> Decoder::decodeString(const string& s) {
    string lengthPart = firstDigit(s);
    size_t digitsInString = lengthPart.size();

    int length = stoi(lengthPart);

    return pair<string, int>(
        s.substr(digitsInString + 1, length),
        digitsInString + 1 + length
    );
}

// Декодирование списка в формате Bencode.
// Формат: "l<элементы>e" (например, "l4:spami42ee" -> ["spam", 42]).
pair<ValueVector, int> Decoder::decodeList(const string& s) {
    ValueVector xs; // Вектор для хранения декодированных элементов списка
    int index = 1;  // Начинаем после 'l'

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') { // Если достигли 'e', завершаем список
            index += 1;
            break;
        }

        int i = 0;
        Value b;
        // Рекурсивно декодируем элемент списка
        std::tie(b, i) = (_decode(s.substr(index)));

        xs.push_back(b); // Добавляем декодированный элемент в список
        index += i;      // Сдвигаем индекс на длину обработанного элемента
    }

    return pair<ValueVector, int>(xs, index); // Возвращаем список и длину обработанной строки
}

// Декодирование словаря в формате Bencode.
// Формат: "d<ключ-значение>e" (например, "d3:cat4:spami42ee" -> {"cat": "spam", "age": 42}).
pair<ValueDictionary, int> Decoder::decodeDict(const string& s) {
    ValueDictionary dict; // Словарь для хранения декодированных пар ключ-значение
    int index = 1;        // Начинаем после 'd'

    while (index < s.length()) {
        char currentChar = s[index];
        if (currentChar == 'e') { // Если достигли 'e', завершаем словарь
            index += 1;
            break;
        }

        Value key;
        int i1;
        Value value;
        int i2;

        // Декодируем ключ (строка)
        std::tie(key, i1) = _decode(s.substr(index));
        // Декодируем значение
        std::tie(value, i2) = _decode(s.substr(index + i1));

        // Добавляем пару ключ-значение в словарь
        dict[boost::get<string>(key)] = value;
        index += i1 + i2; // Сдвигаем индекс на длину обработанных данных
    }

    return pair<ValueDictionary, int>(dict, index); // Возвращаем словарь и длину обработанной строки
}

// Основная вспомогательная функция, которая определяет, как декодировать значение
// (число, строку, список или словарь).
pair<Value, int> Decoder::_decode(const string& s) {
    int index = 0; // Текущий индекс

    // Пары для хранения промежуточных результатов
    pair<Value, int> result;
    pair<ValueVector, int> pairList;
    pair<ValueDictionary, int> pairDict;
    pair<long long, int> pairInt;
    pair<string, int> pairString;

    while (0 < s.length()) {
        char currChar = s[index];

        // Определяем тип значения по первому символу
        switch (currChar) {
        case 'i': // Целое число
            pairInt = decodeInt(s.substr(index)); // Декодируем число
            index += pairInt.second;             // Обновляем индекс
            result = pair<Value, int>(pairInt.first, index); // Упаковываем результат
            return result;

        case 'l': // Список
            pairList = decodeList(s.substr(index)); // Декодируем список
            index += pairList.second;             // Обновляем индекс
            result = pair<Value, int>(pairList.first, index); // Упаковываем результат
            return result;

        case 'd': // Словарь
            pairDict = decodeDict(s.substr(index)); // Декодируем словарь
            index += pairDict.second;             // Обновляем индекс
            result = pair<Value, int>(pairDict.first, index); // Упаковываем результат
            return result;

        default: // Строка
            pairString = decodeString(s.substr(index)); // Декодируем строку
            index += pairString.second;                // Обновляем индекс
            result = pair<Value, int>(pairString.first, index); // Упаковываем результат
            return result;
        }
    }

    return result; // Возвращаем результат (значение и длина обработанной строки)
}

// Основной метод декодирования: вызывает _decode и возвращает декодированное значение.
Value Decoder::decode(const string& string) {
    return _decode(string).first; // Возвращает только значение, игнорируя длину
}
