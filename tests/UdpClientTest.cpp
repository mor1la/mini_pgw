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