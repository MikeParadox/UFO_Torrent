#ifndef DECODE_H
#define DECODE_H

#include "valueTypes.h"

namespace bencode 
{

	class Decoder
	{

	public:

		friend struct Dt;
		static Value decode(const std::string& string);

	private:
		static std::string firstDigit(const std::string& str);

		static std::pair<unsigned long long, int> decodeInt(const std::string& s);

		static std::pair<std::string, int> decodestring(const std::string& s);

		static std::pair<ValueVector, int> decodeList(const std::string& s);

		static std::pair<ValueDictionary, int> decodeDict(const std::string& s);

		static std::pair<Value, int> _decode(const std::string& s);
	};

	struct Dt
	{
		std::pair<ValueDictionary, int> decodeDc(const std::string& s);
		Decoder decoder;
	};
}

#endif