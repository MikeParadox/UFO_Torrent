#include "prettyPrinter.h"
#include "valueTypes.h"
#include <gtest/gtest.h>
#include <sstream>

using namespace bencode;

// Вспомогательный класс для перехвата вывода
class CoutRedirect
{
public:
    CoutRedirect() : old(std::cout.rdbuf(buffer.rdbuf())) {}
    ~CoutRedirect() { std::cout.rdbuf(old); }
    std::string getOutput() const { return buffer.str(); }

private:
    std::streambuf* old;
    std::stringstream buffer;
};

TEST(PrettyPrinterTest, HandlesIntCorrectly)
{
    CoutRedirect redirect;
    PrettyPrinter printer;
    printer(42); // Тестируем вывод числа

    EXPECT_EQ(redirect.getOutput(), "42\n");
}

TEST(PrettyPrinterTest, HandlesAsciiStringCorrectly)
{
    CoutRedirect redirect;
    PrettyPrinter printer;
    printer("test string"); // Тестируем вывод ASCII строки

    EXPECT_EQ(redirect.getOutput(), "test string\n");
}

TEST(PrettyPrinterTest, HandlesBinaryDataString)
{
    CoutRedirect redirect;
    PrettyPrinter printer;
    std::string binary_data = "\x01\x02\x03\x04"; // Не-ASCII данные
    printer(std::string("\x01\x02\x03", 3));      // ??-ASCII ??????
    // restoreCout();

    printer(binary_data); // Должен определить как бинарные

    EXPECT_TRUE(redirect.getOutput().find("BINARY DATA") != std::string::npos);
    EXPECT_TRUE(redirect.getOutput().find("length: 4") != std::string::npos);
}
