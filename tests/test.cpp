#include "ufo_torrent.h"
#include "decode.h"
#include "encode.h"
#include <gtest/gtest.h>
using namespace bencode;


using std::string;
using std::pair;
using std::string;

class Dc_test : public testing::Test
{
protected:
    Dt s{};
};



TEST_F(Dc_test, first_test)
{
    ASSERT_DEATH(s.decodeDc("124444:1231221321"), "Неверный формат");
}


TEST_F(Dc_test, first_test_2)
{
    ASSERT_EQ(Encoder::encode(s.decodeDc("d3:dog4:dogs3:agei8ee").first), "d3:agei8e3:dog4:dogse");
}

TEST_F(Dc_test, first_test_3)
{
    ASSERT_DEATH(s.decodeDc(""), "Неправильный ввод");
}

TEST_F(Dc_test, first_test_4)
{
    ASSERT_EQ(s.decodeDc("d3:cat4:spam3:agei42ee").second, 22);
}

TEST_F(Dc_test, first_test_5)
{
    ASSERT_DEATH(s.decodeDc("d0"), "Неправильный ввод");
}

TEST_F(Dc_test, first_test_6)
{
    ASSERT_DEATH(s.decodeDc("3:kss:dodge3:age67e"), "Неверный ввод");
}

TEST_F(Dc_test, first_test_7)
{
    ASSERT_EQ(Encoder::encode(s.decodeDc("d3:kit6:string3:agei14ee").first), "d3:agei14e3:kit6:stringe");
}

TEST_F(Dc_test, first_test_8)
{
    ASSERT_EQ(Encoder::encode(s.decodeDc("d4:cars5:ldada3:agei192ee").first), "d3:agei192e4:cars5:ldadae");
}
