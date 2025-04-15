#include <gtest/gtest.h>
#include "../src/Encoder.h"
#include "../src/BInteger.h"
#include "../src/BString.h"
#include "../src/BList.h"
#include "../src/BDictionary.h"

namespace bencoding;
TEST(EncoderTest, EncodeInteger) {
    // pos
    auto integer = bencoding::BInteger::create(42);
    EXPECT_EQ(bencoding::encode(integer), "i42e");
    // neg
    auto negativeInt = bencoding::BInteger::create(-10);
    EXPECT_EQ(bencoding::encode(negativeInt), "i-10e");
    // zero
    auto zero = bencoding::BInteger::create(0);
    EXPECT_EQ(bencoding::encode(zero), "i0e");
}

TEST(EncoderTest, EncodeString) {
    // regular line
    auto str = bencoding::BString::create("test");
    EXPECT_EQ(bencoding::encode(str), "4:test");

    // null line
    auto emptyStr = bencoding::BString::create("");
    EXPECT_EQ(bencoding::encode(emptyStr), "0:");
}

TEST(EncoderTest, EncodeList) {
    // list
    auto list = bencoding::BList::create();
    list->push_back(bencoding::BInteger::create(42));
    list->push_back(bencoding::BString::create("foo"));

    EXPECT_EQ(bencoding::encode(list), "li42e3:fooe");
}


