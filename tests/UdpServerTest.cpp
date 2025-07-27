#include "../server/UdpServer.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class UdpServerDecodeTest : public ::testing::Test {
protected:
};

// Обычная корректная BCD-декодировка (чётное количество цифр)
TEST_F(UdpServerDecodeTest, DecodeBcdValidEvenLength) {
    std::string encoded = { static_cast<char>(0x21), 0x43, 0x65 };  // 1,2  3,4  5,6
    std::string result = UdpServer::decodeBcd(encoded);
    EXPECT_EQ(result, "123456");
}

// Корректная BCD-декодировка с заполнителем (нечётное количество цифр)
TEST_F(UdpServerDecodeTest, DecodeBcdValidOddLengthWithFiller) {
    std::string encoded = { static_cast<char>(0x21), 0x43, static_cast<char>(0xF5) };  // 1,2  3,4  F,5
    std::string result = UdpServer::decodeBcd(encoded);
    EXPECT_EQ(result, "12345");
}

// Пустая строка должна вернуть пустую строку
TEST_F(UdpServerDecodeTest, DecodeBcdEmptyStringReturnsEmpty) {
    std::string result = UdpServer::decodeBcd("");
    EXPECT_TRUE(result.empty());
}

// Только 0xFF байты — должно вернуть пустую строку (всё заполнители)
TEST_F(UdpServerDecodeTest, DecodeBcdAllFBytesReturnsEmpty) {
    std::string encoded(3, static_cast<char>(0xFF));
    std::string result = UdpServer::decodeBcd(encoded);
    EXPECT_TRUE(result.empty());
}

// Ошибка: недопустимый low nibble (>9 и не F)
TEST_F(UdpServerDecodeTest, DecodeBcdThrowsOnInvalidLowNibble) {
    std::string encoded = { static_cast<char>(0x2A) };  // 2,10

    EXPECT_THROW({
        UdpServer::decodeBcd(encoded);
    }, std::invalid_argument);
}

// Ошибка: недопустимый high nibble (>9 и не F)
TEST_F(UdpServerDecodeTest, DecodeBcdThrowsOnInvalidHighNibble) {
    std::string encoded = { static_cast<char>(0xA2) };  // 10,2

    EXPECT_THROW({
        UdpServer::decodeBcd(encoded);
    }, std::invalid_argument);
}

// Ошибка: оба nibble недопустимы
TEST_F(UdpServerDecodeTest, DecodeBcdThrowsOnInvalidBothNibbles) {
    std::string encoded = { static_cast<char>(0xAB) };  // 10,11

    EXPECT_THROW({
        UdpServer::decodeBcd(encoded);
    }, std::invalid_argument);
}

// Только low nibble (5), high nibble = F (заполнитель)
TEST_F(UdpServerDecodeTest, DecodeBcdValidLowDigitWithFillerHighNibble) {
    std::string encoded = { static_cast<char>(0xF5) };
    std::string result = UdpServer::decodeBcd(encoded);
    EXPECT_EQ(result, "5");
}

// Только high nibble (5), low nibble = F (заполнитель)
TEST_F(UdpServerDecodeTest, DecodeBcdValidHighDigitWithFillerLowNibble) {
    std::string encoded = { static_cast<char>(0x5F) };
    std::string result = UdpServer::decodeBcd(encoded);
    EXPECT_EQ(result, "5");
}
