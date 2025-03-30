#ifndef ENCODE_H
#define ENCODE_H

#include "valueTypes.h"

#include <string>
using std::string;

namespace bencode {

    class Encoder {
    private:
        Encoder();

    public:
        static string encode(const Value& value);
    };
};

#endif