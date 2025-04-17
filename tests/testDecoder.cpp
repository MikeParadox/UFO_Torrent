#include <gtest/gtest.h>
#include "../src/Decoder.h"

#include <gtest/gtest.h>
#include "../src/Decoder.h"

namespace bencoding {

    class DecoderTest : public ::testing::Test {
    protected:
        std::unique_ptr<Decoder> decoder;

        void SetUp() override {
            decoder = Decoder::create();
        }
    };

    static TEST_F(DecoderTest, Integers) {
        // Valid integers
        ASSERT_EQ(decoder->decode("i42e")->as<BInteger>()->value(), 42);
        ASSERT_EQ(decoder->decode("i-10e")->as<BInteger>()->value(), -10);

        // Invalid integers
        ASSERT_THROW(decoder->decode("i04e"), DecodingError);  // Leading zero not allowed
        ASSERT_THROW(decoder->decode("ie"), DecodingError);    // Empty integer
        ASSERT_THROW(decoder->decode("i-0e"), DecodingError); // Negative zero
    }

    static TEST_F(DecoderTest, DictionarySorting) {
        // Valid (sorted keys)
        auto result = decoder->decode("d1:a1:b2:aa1:ce");
        ASSERT_NE(result, nullptr);

        // Invalid (unsorted keys)
        ASSERT_THROW(decoder->decode("d2:aa1:c1:a1:be"), DecodingError);
    }

}
