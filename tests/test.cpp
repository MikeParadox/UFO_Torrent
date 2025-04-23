#include "ufo_torrent.h"
#include <gtest/gtest.h>

TEST(SumTests, AdditionOfPositiveTests) { EXPECT_EQ(testSum(2, 2), 4); }
TEST(SumTests, AdditionOfPositiveAndZero) { EXPECT_EQ(testSum(2, 0), 2); }