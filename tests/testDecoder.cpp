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

    TEST_F(DecoderTest, Integers) {
        // Valid integers
        EXPECT_EQ(decoder->decode("i42e")->as<BInteger>()->value(), 42);
        EXPECT_EQ(decoder->decode("i-10e")->as<BInteger>()->value(), -10);

        // Invalid integers
        EXPECT_THROW(decoder->decode("i04e"), DecodingError);  // Leading zero not allowed
        EXPECT_THROW(decoder->decode("ie"), DecodingError);    // Empty integer
        EXPECT_THROW(decoder->decode("i-0e"), DecodingError); // Negative zero
    }

    TEST_F(DecoderTest, DictionarySorting) {
        // Valid (sorted keys)
        EXPECT_NO_THROW(decoder->decode("d1:a1:b2:aa1:ce"));

        // Invalid (unsorted keys)
        EXPECT_THROW(decoder->decode("d2:aa1:c1:a1:be"), DecodingError);
    }

}
