#include "ufo_torrent.h"
#include "decode.h"
#include <gtest/gtest.h>
using namespace bencode;



class Dec_test : public testing::Test
{
protected:
    Str d1{};
    Str d2{};
};


TEST_F(Dec_test, general_test)
{
    std::pair<std::string, int> res  { "ubun", 4 };
    ASSERT_EQ(d1.decodeStr("4:ubuntu").first, res.first);
}

TEST_F(Dec_test, general_test_2)
{
    ASSERT_DEATH(d1.decodeStr("i1414196789697899969898989898889:something"), "Выход за диапозон");
}

TEST_F(Dec_test, general_test_3)
{
    ASSERT_DEATH(d1.decodeStr("i0:someth"), "Неверный формат");
}

TEST_F(Dec_test, general_test_4)
{
    std::pair<std::string, int>res{"abcd", 6};
    ASSERT_EQ(d1.decodeStr("4:abcd"), res);
}

TEST_F(Dec_test, general_test_5)
{
    ASSERT_DEATH(d1.decodeStr("i4:qwerty123:key5"), "Неверный формат");
}

TEST_F(Dec_test, general_test_6)
{
    ASSERT_DEATH(d1.decodeStr("i:abcd"), "Неверный формат ввода");
}

TEST_F(Dec_test, general_test_7)
{
    std::pair<std::string, int>res{ "QWET", 6 };
    ASSERT_EQ(d1.decodeStr("4:QWETRT&@"), res);
}

TEST_F(Dec_test, general_test_8)
{
    ASSERT_DEATH(d1.decodeStr("4:").first, "Неверный формат");
}

TEST_F(Dec_test, general_test_9)
{
    std::pair<std::string, int>res{ "some", 6 };
    std::pair<std::string, int>res2{ "some", 6 };
    ASSERT_EQ(d1.decodeStr("4:something"), res);
    ASSERT_EQ(d2.decodeStr("4:some").first, res2.first);
}
TEST_F(Dec_test, general_test_10)
{
    ASSERT_DEATH(d2.decodeStr("4:som"), "Неправильный ввод");
}

TEST_F(Dec_test, general_test_11)
{
    ASSERT_DEATH(d1.decodeStr(":abcd"), "Отсутствует число");
}