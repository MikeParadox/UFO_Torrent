#ifndef DECODE_H
#define DECODE_H

#include "valueTypes.h"

namespace bencode {

	class Decoder
	{
	public:
		// Декодирование строки
		static Value decode(const std::string& string);

	private:
		// первое число в строке
		static std::string firstDigit(const std::string& str);

		// Декодирования первого инта в строке
		static std::pair<long long, int> decodeInt(const std::string& s);

		// Декодирование первого стринга в строке
		static std::pair<std::string, int> decodeString(const std::string& s);

		// Декодирования листа
		static std::pair<ValueVector, int> decodeList(const std::string& s);

		// Декодирования словаря
		static std::pair<ValueDictionary, int> decodeDict(const std::string& s);

		// Декодирования всей строки рекурсией
		static std::pair<Value, int> _decode(const std::string& s);
	};
}

#endif