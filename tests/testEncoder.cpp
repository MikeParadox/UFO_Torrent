#include <gtest/gtest.h>
#include "../src/Encoder.h"
#include "../src/BInteger.h"
#include "../src/BString.h"
#include "../src/BList.h"
#include "../src/BDictionary.h"

namespace bencoding;
namespace {

	class EncoderTest : public ::testing::Test {
	protected:
		void SetUp() override {
			encoder = bencoding::Encoder::create();
		}

		std::unique_ptr<bencoding::Encoder> encoder;
	};

	TEST_F(EncoderTest, EncodesPositiveInteger) {
		auto integer = bencoding::BInteger::create(42);
		ASSERT_EQ(encoder->encode(integer), "i42e");
	}

	TEST_F(EncoderTest, EncodesNegativeInteger) {
		auto integer = bencoding::BInteger::create(-10);
		ASSERT_EQ(encoder->encode(integer), "i-10e");
	}

	TEST_F(EncoderTest, EncodesZero) {
		auto integer = bencoding::BInteger::create(0);
		ASSERT_EQ(encoder->encode(integer), "i0e");
	}

	TEST_F(EncoderTest, EncodesRegularString) {
		auto str = bencoding::BString::create("test");
		ASSERT_EQ(encoder->encode(str), "4:test");
	}

	TEST_F(EncoderTest, EncodesEmptyString) {
		auto str = bencoding::BString::create("");
		ASSERT_EQ(encoder->encode(str), "0:");
	}

	TEST_F(EncoderTest, EncodesSimpleList) {
		auto list = bencoding::BList::create();
		list->push_back(bencoding::BInteger::create(42));
		list->push_back(bencoding::BString::create("foo"));
		ASSERT_EQ(encoder->encode(list), "li42e3:fooe");
	}

	TEST_F(EncoderTest, EncodesSimpleDictionary) {
		auto dict = bencoding::BDictionary::create();
		dict->insert(bencoding::BString::create("foo"), bencoding::BString::create("bar"));
		dict->insert(bencoding::BString::create("num"), bencoding::BInteger::create(42));
		ASSERT_EQ(encoder->encode(dict), "d3:foo3:bar3:numi42ee");
	}

	TEST_F(EncoderTest, EncodesNestedStructures) {
		auto inner_list = bencoding::BList::create();
		inner_list->push_back(bencoding::BString::create("hello"));

		auto dict = bencoding::BDictionary::create();
		dict->insert(bencoding::BString::create("list"), inner_list);

		ASSERT_EQ(encoder->encode(dict), "d4:listl5:helloee");
	}

} 