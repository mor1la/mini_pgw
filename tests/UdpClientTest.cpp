#include "../client/UdpClient.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class UdpClientEncodeTest : public ::testing::Test {
protected:
};

// IMSI с чётным количеством цифр
TEST_F(UdpClientEncodeTest, EncodeBcdCorrectlyEncodesImsiWithEvenDigits) {
    std::string imsi = "123456";
    std::string encoded = UdpClient::encodeBcd(imsi);

    ASSERT_EQ(encoded.size(), 3);
    EXPECT_EQ((uint8_t)encoded[0], 0x21);  // 1 + 2
    EXPECT_EQ((uint8_t)encoded[1], 0x43);  // 3 + 4
    EXPECT_EQ((uint8_t)encoded[2], 0x65);  // 5 + 6
}

// IMSI с нечётным количеством цифр (последний байт должен содержать 0xF)
TEST_F(UdpClientEncodeTest, EncodeBcdCorrectlyEncodesImsiWithOddDigits) {
    std::string imsi = "12345";
    std::string encoded = UdpClient::encodeBcd(imsi);

    ASSERT_EQ(encoded.size(), 3);
    EXPECT_EQ((uint8_t)encoded[0], 0x21);  // 1 + 2
    EXPECT_EQ((uint8_t)encoded[1], 0x43);  // 3 + 4
    EXPECT_EQ((uint8_t)encoded[2], 0xF5);  // 5 + padding
}

// Пустая строка должна возвращать пустой результат
TEST_F(UdpClientEncodeTest, EncodeBcdHandlesEmptyString) {
    std::string imsi = "";
    std::string encoded = UdpClient::encodeBcd(imsi);
    ASSERT_TRUE(encoded.empty());
}

// IMSI с одним символом — должен дополняться 0xF
TEST_F(UdpClientEncodeTest, EncodeBcdHandlesSingleDigitImsi) {
    std::string imsi = "7";
    std::string encoded = UdpClient::encodeBcd(imsi);
    ASSERT_EQ(encoded.size(), 1);
    EXPECT_EQ((uint8_t)encoded[0], 0xF7);  // Padding + 7
}

// IMSI с максимальной допустимой длиной (например, 15 цифр, как в стандарте IMSI)
TEST_F(UdpClientEncodeTest, EncodeBcdHandlesMaxImsiLength) {
    std::string imsi = "123456789012345";
    std::string encoded = UdpClient::encodeBcd(imsi);
    ASSERT_EQ(encoded.size(), 8);  // (15+1)/2 = 8 байт
    EXPECT_EQ((uint8_t)encoded[0], 0x21);  // 1 + 2
    EXPECT_EQ((uint8_t)encoded[1], 0x43);  // 3 + 4
    EXPECT_EQ((uint8_t)encoded[2], 0x65);  // 5 + 6
    EXPECT_EQ((uint8_t)encoded[3], 0x87);  // 7 + 8
    EXPECT_EQ((uint8_t)encoded[4], 0x09);  // 9 + 0
    EXPECT_EQ((uint8_t)encoded[5], 0x21);  // 1 + 2
    EXPECT_EQ((uint8_t)encoded[6], 0x43);  // 3 + 4
    EXPECT_EQ((uint8_t)encoded[7], 0xF5);  // 5 + padding
}

// IMSI с недопустимыми символами — должен выбрасывать исключение
TEST_F(UdpClientEncodeTest, EncodeBcdThrowsOnInvalidCharacters) {
    std::string imsi = "12A45";
    EXPECT_THROW(UdpClient::encodeBcd(imsi), std::invalid_argument);
}

// IMSI с пробелами или спецсимволами — должен выбрасывать исключение
TEST_F(UdpClientEncodeTest, EncodeBcdThrowsOnSpecialCharacters) {
    std::string imsi = "12 34";
    EXPECT_THROW(UdpClient::encodeBcd(imsi), std::invalid_argument);

    imsi = "12-34";
    EXPECT_THROW(UdpClient::encodeBcd(imsi), std::invalid_argument);
}
