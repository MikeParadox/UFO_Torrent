#include <gtest/gtest.h>
#include "../includes/prettyPrinter.h"
#include <sstream>

class PrettyPrinterTest : public ::testing::Test {
protected:
    void redirectCout() {
        old = std::cout.rdbuf();
        std::cout.rdbuf(buffer.rdbuf());
    }

    void restoreCout() {
        std::cout.rdbuf(old);
    }

    std::stringstream buffer;
    std::streambuf* old;
};

TEST_F(PrettyPrinterTest, PrintsInteger) {
    redirectCout();
    PrettyPrinter printer;
    printer(42);
    restoreCout();

    EXPECT_EQ(buffer.str(), "42\n");
}

TEST_F(PrettyPrinterTest, PrintsAsciiString) {
    redirectCout();
    PrettyPrinter printer;
    printer("test string");
    restoreCout();

    EXPECT_EQ(buffer.str(), "test string\n");
}

TEST_F(PrettyPrinterTest, PrintsBinaryDataNotice) {
    redirectCout();
    PrettyPrinter printer;
    printer(std::string("\x01\x02\x03", 3)); // Не-ASCII данные
    restoreCout();

    EXPECT_EQ(buffer.str(), "BINARY DATA (length: 3)\n");
}

TEST_F(PrettyPrinterTest, HandlesDictionary) {
    redirectCout();
    PrettyPrinter printer;
    ValueDictionary dict;
    dict["key"] = 123;
    printer(dict);
    restoreCout();

    EXPECT_NE(buffer.str().find("{key}: 123"), std::string::npos);
}

TEST_F(PrettyPrinterTest, HandlesVector) {
    redirectCout();
    PrettyPrinter printer;
    ValueVector vec;
    vec.push_back("test");
    printer(vec);
    restoreCout();

    EXPECT_NE(buffer.str().find("[0]: test"), std::string::npos);
}