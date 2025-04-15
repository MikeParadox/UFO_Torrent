#include <gtest/gtest.h>
#include "../src/Decoder.h"

namespace bencoding;  // Замените на реальное пространство имён

TEST(DecoderTest, Integers) {
    auto decoder = bencoding::Decoder::create();

    // Valid integers
    EXPECT_EQ(decoder->decode("i42e")->as<bencoding::BInteger>()->value(), 42);
    EXPECT_EQ(decoder->decode("i-10e")->as<bencoding::BInteger>()->value(), -10);

    // Invalid integers
    EXPECT_THROW(decoder->decode("i04e"), bencoding::DecodingError);  // 0
    EXPECT_THROW(decoder->decode("ie"), bencoding::DecodingError);   // пустая строка
}

TEST(DecoderTest, DictionarySorting) {
    auto decoder = bencoding::Decoder::create();

    // Valid (sorted keys)
    EXPECT_NO_THROW(decoder->decode("d1:a1:b2:aa1:ce"));

    // Invalid (unsorted keys)
    EXPECT_THROW(decoder->decode("d2:aa1:c1:a1:be"), bencoding::DecodingError);
}
