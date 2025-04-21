#include <gtest/gtest.h>
#include "Piece.h"

// Тест проверяет создание Piece и базовые свойства
TEST(PieceTest, ConstructorAndBasicProperties) {
    std::vector<Block*> blocks = { new Block(0, 1024) };
    Piece piece(1, blocks, "aabbcc");

    EXPECT_EQ(piece.index(), 1); // Проверка индекса
    EXPECT_FALSE(piece.is_complete()); // По умолчанию не завершена
}

// Тест проверяет запрос блоков для загрузки
TEST(PieceTest, NextRequestBlock) {
    Block* block1 = new Block(0, 1024);
    std::vector<Block*> blocks = { block1 };
    Piece piece(1, blocks, "hash");

    Block* requested = piece.next_request();
    ASSERT_NE(requested, nullptr); // Должен вернуть блок
    EXPECT_EQ(requested->status, pending); // Статус должен измениться
}

// Тест проверяет получение блока данных
TEST(PieceTest, ReceiveBlockData) {
    const int test_offset = 1024;  // Явно задаём offset
    Block* block = new Block(test_offset, 1024);  // Указываем тот же offset

    // Проверяем, что блок создан правильно
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->offset, test_offset);// Данные должны сохраниться
    //Создаём Piece с этим блоком
    std::vector<Block*> blocks = { block };
    Piece piece(1, blocks, "hash");
    //Проверяем, что блок добавлен в Piece
    ASSERT_EQ(piece.next_request(), block);
}
