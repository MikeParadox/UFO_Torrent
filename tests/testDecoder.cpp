#include <gtest/gtest.h>
#include "../bencode/Decoder.h"
#include "../bencode/BInteger.h"  
#include "../bencode/BItem.h"  

namespace bencoding {

    class DecoderTest : public ::testing::Test {
    protected:
        std::unique_ptr<Decoder> decoder;
        void SetUp() override {
            decoder = Decoder::create();
        }
    };
    TEST_F(DecoderTest, DictionarySorting) {
        // Test valid dictionary decoding
        auto result = decoder->decode("d1:a1:b2:aa1:ce");
        ASSERT_NE(result, nullptr);

        // Test invalid dictionary format
        ASSERT_THROW(decoder->decode("dinvalid"), DecodingError);
    }
    TEST_F(DecoderTest, Integers)
    {
        // First verify decoder was created successfully
        ASSERT_NE(decoder, nullptr) << "Decoder creation failed";
        // Test decoding of valid integer "i42e" (bencoded 42)
        auto decoded_item = decoder->decode("i42e");
        ASSERT_NE(decoded_item, nullptr) << "Decoding failed";
        // Try to cast decoded item to BInteger*
        BInteger* intr = dynamic_cast<BInteger*>(decoded_item.get());
        ASSERT_NE(intr, nullptr) << "Not a BInteger object" << typeid(*decoded_item).name();
        // Verify the integer value matches expected 42
        ASSERT_EQ(intr->value(), 42) << "Wrong integer value";
    }
}