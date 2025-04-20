#include <gtest/gtest.h>
#include "../bencode/Decoder.h"
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
    //    // Valid (sorted keys)
        auto result = decoder->decode("d1:a1:b2:aa1:ce");
        ASSERT_NE(result, nullptr);

        // Invalid text
        ASSERT_THROW(decoder->decode("dinvalid"), DecodingError);
    }
    TEST_F(DecoderTest, Integers)
    {
        ASSERT_NE(decoder, nullptr) << "Decoder creation failed";

        auto decoded_item = decoder->decode("i42e");
        ASSERT_NE(decoded_item, nullptr) << "Decoding failed";

        BInteger* intr = dynamic_cast<BInteger*>(decoded_item.get());
        ASSERT_NE(intr, nullptr) << "Not a BInteger object" << typeid(*decoded_item).name();
 
        ASSERT_EQ(intr->value(), 42) << "Wrong integer value";
    }
}