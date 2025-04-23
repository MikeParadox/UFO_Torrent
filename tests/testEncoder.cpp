#include <gtest/gtest.h>
#include "../bencode/Encoder.h"
#include "../bencode/BInteger.h"
#include "../bencode/BString.h"
#include "../bencode/BList.h"
#include "../bencode/BDictionary.h"


namespace bencoding {

	class EncoderTest : public ::testing::Test {
	protected:
		void SetUp() override {
			encoder = bencoding::Encoder::create();
		}

		std::unique_ptr<bencoding::Encoder> encoder;
	};

	TEST_F(EncoderTest, PositiveInteger) {
		// Test basic positive integer encoding
		auto integer = BInteger::create(42);
		std::string result = encoder->encode(std::shared_ptr<BItem>(integer.release()));
		ASSERT_EQ(result, "i42e") << "Positive integer encoding failed";
	}

	// Test negative integer encoding
	TEST_F(EncoderTest, NegativeInteger) {
		auto integer = BInteger::create(-123);
		std::string result = encoder->encode(std::shared_ptr<BItem>(integer.release()));
		ASSERT_EQ(result, "i-123e") << "Negative integer encoding failed";
	}

	// Test empty string encoding
	TEST_F(EncoderTest, EmptyString) {
		auto str = BString::create("");
		std::string result = encoder->encode(std::shared_ptr<BItem>(str.release()));
		ASSERT_EQ(result, "0:") << "Empty string encoding failed";
	}

	// Test non-empty string encoding
	TEST_F(EncoderTest, NonEmptyString) {
		auto str = BString::create("test");
		std::string result = encoder->encode(std::shared_ptr<BItem>(str.release()));
		ASSERT_EQ(result, "4:test") << "Non-empty string encoding failed";
	}

	// Test list with multiple elements encoding
	TEST_F(EncoderTest, NonEmptyList) {
		auto list = BList::create();
		list->push_back(BInteger::create(1));
		list->push_back(BString::create("abc"));
		std::string result = encoder->encode(std::shared_ptr<BItem>(list.release()));
		ASSERT_EQ(result, "li1e3:abce") << "Non-empty list encoding failed";
	}

	// Test empty dictionary encoding
	TEST_F(EncoderTest, EmptyDictionary) {
		auto dict = BDictionary::create();
		std::string result = encoder->encode(std::shared_ptr<BItem>(dict.release()));
		ASSERT_EQ(result, "de") << "Empty dictionary encoding failed";
	}
} 