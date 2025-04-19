#include <gtest/gtest.h>
#include "Piece.h"

class PieceTest : public ::testing::Test {
protected:
    void SetUp() override {
        blocks.push_back(new Block{ 0, 1024, missing, "" });
        blocks.push_back(new Block{ 1024, 1024, missing, "" });
        test_hash = "1234567890abcdef12345678";
    }

    void TearDown() override {
        for (auto block : blocks) delete block;
    }

    std::vector<Block*> blocks;
    std::string test_hash;
};

// 1. ������� ���� ������������
TEST_F(PieceTest, Constructor) {
    Piece piece(1, blocks, test_hash);
    EXPECT_EQ(piece.index(), 1);
    EXPECT_EQ(piece.blocks().size(), 2);
}

// 2. ���� ������� � ��������� ������ (���������� next_request � block_received)
TEST_F(PieceTest, BlockRequestAndReceive) {
    Piece piece(1, blocks, test_hash);

    // ����������� ������ ����
    Block* block = piece.next_request();
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->offset, 0);
    EXPECT_EQ(block->status, pending);

    // �������� ��� ����������
    piece.block_received(block->offset, "test");
    EXPECT_EQ(block->status, retrieved);
    EXPECT_EQ(block->data, "test");
}

// 3. ���� �������� �������������
TEST_F(PieceTest, CompletionCheck) {
    Piece piece(1, blocks, test_hash);

    // �������� ��� �����
    while (auto block = piece.next_request()) {
        piece.block_received(block->offset, "data");
    }

    EXPECT_TRUE(piece.is_complete());
}

// 4. ���� ������ ���������
TEST_F(PieceTest, Reset) {
    Piece piece(1, blocks, test_hash);

    // �������� ���� � ����������
    piece.next_request();
    piece.reset();

    for (auto block : blocks) {
        EXPECT_EQ(block->status, missing);
    }
}