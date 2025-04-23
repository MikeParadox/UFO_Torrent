#include <gtest/gtest.h>
#include "Piece.h"

// ���� ��������� �������� Piece � ������� ��������
TEST(PieceTest, ConstructorAndBasicProperties) {
    std::vector<Block*> blocks = { new Block(0, 1024) };
    Piece piece(1, blocks, "aabbcc");

    EXPECT_EQ(piece.index(), 1); // �������� �������
    EXPECT_FALSE(piece.is_complete()); // �� ��������� �� ���������
}

// ���� ��������� ������ ������ ��� ��������
TEST(PieceTest, NextRequestBlock) {
    Block* block1 = new Block(0, 1024);
    std::vector<Block*> blocks = { block1 };
    Piece piece(1, blocks, "hash");

    Block* requested = piece.next_request();
    ASSERT_NE(requested, nullptr); // ������ ������� ����
    EXPECT_EQ(requested->status, pending); // ������ ������ ����������
}

// ���� ��������� ��������� ����� ������
TEST(PieceTest, ReceiveBlockData) {
    const int test_offset = 1024;  // ���� ����� offset
    Block* block = new Block(test_offset, 1024);  // ��������� ��� �� offset

    // ���������, ��� ���� ������ ���������
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->offset, test_offset);// ������ ������ �����������
    //������ Piece � ���� ������
    std::vector<Block*> blocks = { block };
    Piece piece(1, blocks, "hash");
    //���������, ��� ���� �������� � Piece
    ASSERT_EQ(piece.next_request(), block);
}
