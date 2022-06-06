#include "TestTilesets.hpp"

#include <iostream>

#define WFC_CONCAT(...) __VA_ARGS__

TEST(Sanity)
{
    CHECK_EQUAL(1, 1);
}

SUITE(Utils)
{
    using namespace WFC::Math;

    TEST(SmallestUInt)
    {
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<1>, uint_least8_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<2>, uint_least8_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<6>, uint_least8_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<7>, uint_least8_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<8>, uint_least8_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<9>, uint_least16_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<11>, uint_least16_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<12>, uint_least16_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<15>, uint_least16_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<16>, uint_least16_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<17>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<18>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<20>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<20>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<25>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<27>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<30>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<31>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<32>, uint_least32_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<33>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<36>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<39>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<42>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<47>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<52>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<57>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<61>, uint_least64_t)>);
        CHECK(std::is_same_v<WFC_CONCAT(SmallestUInt<64>, uint_least64_t)>);
    }

    TEST(FindBitIndex)
    {
        CHECK_EQUAL(0, FindBitIndex((uint32_t)0b1));
        CHECK_EQUAL(1, FindBitIndex((uint32_t)0b10));
        CHECK_EQUAL(2, FindBitIndex((uint32_t)0b100));
        CHECK_EQUAL(5, FindBitIndex((uint32_t)0b100000));
        CHECK_EQUAL(11, FindBitIndex((uint32_t)0b100000000000));
        CHECK_EQUAL(16, FindBitIndex((uint32_t)0b10000000000000000));
        CHECK_EQUAL(17, FindBitIndex((uint32_t)0b100000000000000000));
        CHECK_EQUAL(23, FindBitIndex((uint32_t)0b100000000000000000000000));
        CHECK_EQUAL(30, FindBitIndex((uint32_t)0b1000000000000000000000000000000));
        CHECK_EQUAL(31, FindBitIndex((uint32_t)0b10000000000000000000000000000000));
    }
    
    TEST(CountBitsPt1)
    {
        CHECK_EQUAL(0, CountBits((uint8_t)0b0));
        CHECK_EQUAL(2, CountBits((uint8_t)0b0110));
        CHECK_EQUAL(2, CountBits((uint8_t)0b1001));
        CHECK_EQUAL(3, CountBits((uint8_t)0b1110));
        CHECK_EQUAL(2, CountBits((uint8_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint8_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint8_t)0b11100000));
        CHECK_EQUAL(4, CountBits((uint8_t)0b10101010));
        CHECK_EQUAL(8, CountBits((uint8_t)0b11111111));

        CHECK_EQUAL(0, CountBits((uint16_t)0b0));
        CHECK_EQUAL(2, CountBits((uint16_t)0b0110));
        CHECK_EQUAL(2, CountBits((uint16_t)0b1001));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1110));
        CHECK_EQUAL(2, CountBits((uint16_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b11100000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b11100000));
        CHECK_EQUAL(4, CountBits((uint16_t)0b10101010));
        CHECK_EQUAL(8, CountBits((uint16_t)0b11111111));
        CHECK_EQUAL(2, CountBits((uint16_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint16_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint16_t)0b1111111100000000));
        CHECK_EQUAL(5, CountBits((uint16_t)0b0110001110000000));
        CHECK_EQUAL(6, CountBits((uint16_t)0b1001000001111000));
        CHECK_EQUAL(4, CountBits((uint16_t)0b1110001000000000));
        CHECK_EQUAL(11, CountBits((uint16_t)0b0110000111111111));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1001000000000001));
        CHECK_EQUAL(6, CountBits((uint16_t)0b1110010000100010));
        CHECK_EQUAL(2, CountBits((uint16_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint16_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint16_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint16_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint16_t)0b1111111100000000));
        CHECK_EQUAL(16, CountBits((uint16_t)0b1111111111111111));
    }
    TEST(CountBitsPt2)
    {
        CHECK_EQUAL(0, CountBits((uint32_t)0b0));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b10101010));
        CHECK_EQUAL(8, CountBits((uint32_t)0b11111111));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b1111111100000000));
        CHECK_EQUAL(5, CountBits((uint32_t)0b0110001110000000));
        CHECK_EQUAL(6, CountBits((uint32_t)0b1001000001111000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b1110001000000000));
        CHECK_EQUAL(11, CountBits((uint32_t)0b0110000111111111));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1001000000000001));
        CHECK_EQUAL(6, CountBits((uint32_t)0b1110010000100010));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b1111111100000000));
        CHECK_EQUAL(6, CountBits((uint32_t)0b1110010000100010));
        CHECK_EQUAL(2, CountBits((uint32_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b1111111100000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b0000000011111111));
        CHECK_EQUAL(16, CountBits((uint32_t)0b1111111111111111));
    }
    TEST(CountBitsPart3)
    {
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(5, CountBits((uint32_t)0b01100011100000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint32_t)0b10010000011110000000000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b11100010000000000000000000000000));
        CHECK_EQUAL(11, CountBits((uint32_t)0b01100001111111110000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b10010000000000010000000000000000));
        CHECK_EQUAL(6, CountBits((uint32_t)0b11100100001000100000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint32_t)0b11100100001000100000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint32_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint32_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint32_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint32_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(16, CountBits((uint32_t)0b11111111111111110000000000000000));
        CHECK_EQUAL(10, CountBits((uint32_t)0b11100000000000000011111110000000));
        CHECK_EQUAL(17, CountBits((uint32_t)0b10101010000011100111001110011101));
        CHECK_EQUAL(20, CountBits((uint32_t)0b11111111110000110110001111000110));
        CHECK_EQUAL(16, CountBits((uint32_t)0b11111111111111110000000000000000));
        CHECK_EQUAL(16, CountBits((uint32_t)0b00000000000000001111111111111111));
        CHECK_EQUAL(32, CountBits((uint32_t)0b11111111111111111111111111111111));
    }
    TEST(CountBitsPt4)
    {
        CHECK_EQUAL(0, CountBits((uint64_t)0b0));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b10101010));
        CHECK_EQUAL(8, CountBits((uint64_t)0b11111111));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000));
        CHECK_EQUAL(5, CountBits((uint64_t)0b0110001110000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1001000001111000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1110001000000000));
        CHECK_EQUAL(11, CountBits((uint64_t)0b0110000111111111));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1001000000000001));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1110010000100010));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1110010000100010));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b0000000011111111));
        CHECK_EQUAL(16, CountBits((uint64_t)0b1111111111111111));
    }
    TEST(CountBitsPt5)
    {
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(5, CountBits((uint64_t)0b01100011100000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b10010000011110000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b11100010000000000000000000000000));
        CHECK_EQUAL(11, CountBits((uint64_t)0b01100001111111110000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b10010000000000010000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b11100100001000100000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b11100100001000100000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b01100000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b10010000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b11100000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b10101010000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b11111111000000000000000000000000));
        CHECK_EQUAL(16, CountBits((uint64_t)0b11111111111111110000000000000000));
        CHECK_EQUAL(10, CountBits((uint64_t)0b11100000000000000011111110000000));
        CHECK_EQUAL(17, CountBits((uint64_t)0b10101010000011100111001110011101));
        CHECK_EQUAL(20, CountBits((uint64_t)0b11111111110000110110001111000110));
        CHECK_EQUAL(16, CountBits((uint64_t)0b11111111111111110000000000000000));
        CHECK_EQUAL(16, CountBits((uint64_t)0b00000000000000001111111111111111));
        CHECK_EQUAL(32, CountBits((uint64_t)0b11111111111111111111111111111111));
    }
    TEST(CountBitsPt6)
    {
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(5, CountBits((uint64_t)0b0110001110000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1001000001111000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1110001000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(11, CountBits((uint64_t)0b0110000111111111000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1001000000000001000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1110010000100010000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(6, CountBits((uint64_t)0b1110010000100010000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(2, CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(3, CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(4, CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(8, CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(16, CountBits((uint64_t)0b1111111111111111000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(10, CountBits((uint64_t)0b1110000000000000001111111000000000000000000000000000000000000000));
        CHECK_EQUAL(17, CountBits((uint64_t)0b1010101000001110011100111001110100000000000000000000000000000000));
        CHECK_EQUAL(20, CountBits((uint64_t)0b1111111111000011011000111100011000000000000000000000000000000000));
        CHECK_EQUAL(16, CountBits((uint64_t)0b1111111111111111000000000000000000000000000000000000000000000000));
        CHECK_EQUAL(16, CountBits((uint64_t)0b0000000000000000111111111111111100000000000000000000000000000000));
        CHECK_EQUAL(32, CountBits((uint64_t)0b1111111111111111111111111111111100000000000000000000000000000000));
    }

    TEST(PositiveModulo)
    {
        CHECK_EQUAL(5, PositiveModulo(5, 20));
        CHECK_EQUAL(2, PositiveModulo(5, 3));
        CHECK_EQUAL(2, PositiveModulo(5, 3));
        CHECK_EQUAL(1, PositiveModulo(-5, 2));
        CHECK_EQUAL(0, PositiveModulo(-6, 2));
        CHECK_EQUAL(3, PositiveModulo(-13, 4));
    }
}

SUITE(WFC_Simple)
{
    using namespace Tilesets::Simple;
}

SUITE(WFC_Tiled)
{
    using namespace Tilesets::Tiled;
}

SUITE(WFC_Tiled3D)
{
    using namespace Tilesets::Tiled3D;

    size_t CountSetIterator(const TransformSet& set)
    {
        size_t count = 0;
        for (Transform3D tr : set)
            count += 1;
        return count;
    }
    std::vector<Transform3D> ReadSet(const TransformSet& set)
    {
        std::vector<Transform3D> output;
        output.resize(set.Size());
        std::copy(set.begin(), set.end(), output.begin());
        return output;
    }

    TEST(TransformPoint)
    {
        //Try some simple point transformations.
        Vector3i testPos(1, 3, 5);

        CHECK(Transform3D{ false }.ApplyToPos(testPos)
                == testPos);
        CHECK(Transform3D{ true }.ApplyToPos(testPos)
                == -testPos);
        CHECK((Transform3D{WFC_CONCAT(false, Rotations3D::EdgesZb)}).ApplyToPos(testPos)
                == -Vector3i(WFC_CONCAT(testPos.y, testPos.x, testPos.z)));
    }

    TEST(TransformInversion)
    {
        //For every possible transform, try applying it and then its inverse,
        //    and check that the output is unchanged.
        Vector3i testPos(1, 3, 5);

        for (int invert = 0; invert < 2; ++invert)
            for (int rotI = 0; rotI < N_ROTATIONS_3D; ++rotI)
            {
                Transform3D forward = { invert == 1, (Rotations3D)rotI },
                            inverse = forward.Inverse();
                CHECK_EQUAL(testPos, inverse.ApplyToPos(forward.ApplyToPos(testPos)));
                CHECK_EQUAL(testPos, forward.ApplyToPos(inverse.ApplyToPos(testPos)));
            }
    }

    // NOTE: Tests of Tiled3D::GetFace() would be nice, but it's self-testing with asserts
    //    so not high-priority.

    static const Transform3D setTransforms[] = {
        //Defined in their expected order.
        { false, Rotations3D::None },
        { false, Rotations3D::CornerBAA_120 },
        { true, Rotations3D::None },
        { true, Rotations3D::AxisZ_180 },
        { true, Rotations3D::CornerBAA_120 }
    };
    TEST(TransformSetPart1)
    {
        TransformSet set;

        CHECK_EQUAL(0, set.Size());
        CHECK_EQUAL(0, CountSetIterator(set));
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        auto vec = ReadSet(set);
        CHECK_EQUAL(0, vec.size());

        set.Add(setTransforms[0]);
        CHECK_EQUAL(1, set.Size());
        CHECK_EQUAL(1, CountSetIterator(set));
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK_EQUAL(1, vec.size());
        CHECK_EQUAL(setTransforms[0], vec[0]);

        set.Add(setTransforms[2]);
        CHECK_EQUAL(2, set.Size());
        CHECK_EQUAL(2, CountSetIterator(set));
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK_EQUAL(2, vec.size());
        CHECK_EQUAL(setTransforms[0], vec[0]);
        CHECK_EQUAL(setTransforms[2], vec[1]);

        set.Remove(setTransforms[0]);
        CHECK_EQUAL(1, set.Size());
        CHECK_EQUAL(1, CountSetIterator(set));
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK_EQUAL(1, vec.size());
        CHECK_EQUAL(setTransforms[2], vec[0]);

        set.Remove(setTransforms[2]);
        CHECK_EQUAL(0, set.Size());
        CHECK_EQUAL(0, CountSetIterator(set));
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK_EQUAL(0, vec.size());
    }
    TEST(TransformSetPart2)
    {
        TransformSet set;

        auto set1 = TransformSet::Combine(setTransforms[0], setTransforms[3]);
        auto set2 = TransformSet::Combine(setTransforms[2], setTransforms[4]);
        set = TransformSet::Combine(set1, set2);
        CHECK_EQUAL(4, set.Size());
        CHECK_EQUAL(4, CountSetIterator(set));
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(set.Contains(setTransforms[3]));
        CHECK(set.Contains(setTransforms[4]));
        auto vec = ReadSet(set);
        CHECK_EQUAL(4, vec.size());
        CHECK_EQUAL(setTransforms[0], vec[0]);
        CHECK_EQUAL(setTransforms[2], vec[1]);
        CHECK_EQUAL(setTransforms[3], vec[2]);
        CHECK_EQUAL(setTransforms[4], vec[3]);

        CHECK(set1 != set2);
        CHECK_EQUAL(TransformSet::Combine(set1, set2), set);
        CHECK_EQUAL(TransformSet::Combine(set2, set1), set);

        set = set1;
        CHECK_EQUAL(2, set.Add(set2));
        CHECK_EQUAL(TransformSet::Combine(set1, set2), set);
        CHECK_EQUAL(0, set.Add(set2));
        CHECK_EQUAL(TransformSet::Combine(set1, set2), set);

        set = set1;
        CHECK_EQUAL(2, set.Remove(set1));
        CHECK_EQUAL(TransformSet{}, set);
        CHECK_EQUAL(0, set.Remove(set1));
        CHECK_EQUAL(TransformSet{}, set);

        set = TransformSet::Combine(set1, set2);
        CHECK_EQUAL(2, set.Intersect(set2));
        CHECK_EQUAL(set2, set);
        CHECK_EQUAL(0, set.Intersect(set2));
        CHECK_EQUAL(set2, set);
    }
    TEST(TransformSetPart3)
    {
        TransformSet set;
        set.Add(Transform3D{ });
        set.Add(Transform3D{ false, Rotations3D::AxisX_90 });
        set.Add(Transform3D{ false, Rotations3D::CornerBBA_240 });
        set.AddInvertedVersions();

        CHECK_EQUAL(6, set.Size());
        CHECK(set.Contains(Transform3D{ true }));
        CHECK(set.Contains(Transform3D{ true, Rotations3D::AxisX_90 }));
        CHECK(set.Contains(Transform3D{ true, Rotations3D::CornerBBA_240 }));
        CHECK(!set.Contains(Transform3D{ true, Rotations3D::AxisX_180 }));
        auto vec = ReadSet(set);
        CHECK_EQUAL(6, vec.size());
        CHECK_EQUAL(Transform3D{ }, vec[0]);
        CHECK_EQUAL(Transform3D{WFC_CONCAT(false, Rotations3D::AxisX_90)}, vec[1]);
        CHECK_EQUAL(Transform3D{WFC_CONCAT(false, Rotations3D::CornerBBA_240)}, vec[2]);
        CHECK_EQUAL(Transform3D{ true }, vec[3]);
        CHECK_EQUAL(Transform3D{WFC_CONCAT(true, Rotations3D::AxisX_90)}, vec[4]);
        CHECK_EQUAL(Transform3D{WFC_CONCAT(true, Rotations3D::CornerBBA_240)}, vec[5]);

        //Check that existing inverted rotations are not affected by 'AddInvertedRotations()'.
        set.Clear();
        set.Add(Transform3D{ false, Rotations3D::AxisZ_270 });
        set.Add(Transform3D{ true, Rotations3D::EdgesYa });
        set.AddInvertedVersions();

        CHECK_EQUAL(3, set.Size());
        CHECK(set.Contains(Transform3D{ false, Rotations3D::AxisZ_270 }));
        CHECK(set.Contains(Transform3D{ true, Rotations3D::AxisZ_270 }));
        CHECK(set.Contains(Transform3D{ true, Rotations3D::EdgesYa }));
        vec = ReadSet(set);
        CHECK_EQUAL(3, vec.size());
        CHECK_EQUAL(Transform3D{ WFC_CONCAT(false, Rotations3D::AxisZ_270) }, vec[0]);
        CHECK_EQUAL(Transform3D{ WFC_CONCAT(true, Rotations3D::AxisZ_270) }, vec[1]);
        CHECK_EQUAL(Transform3D{ WFC_CONCAT(true, Rotations3D::EdgesYa) }, vec[2]);
    }

    TEST(StateInitialization)
    {
        //Use inverted and/or sideways permutations of the single-tile tileset.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.AddInvertedVersions();
        //TODO: Add one permutation that doesn't affect the X faces.
        State state(OneTileArmy(usedTransforms), { 1, 2, 3 });

        CHECK_EQUAL(1, state.InputTiles.GetSize());
        CHECK_EQUAL(4, state.NPermutedTiles);

        //The Z faces are entirely symmetrical, so the rotation/inversion
        //    should not change them.
        //The same does not hold for the X or Y faces.
        CHECK_EQUAL(2 + (4 * 4), state.FaceIndices.GetSize());

        //Check that the input tile data transferred over correctly.
        CHECK_EQUAL(Vector4i{WFC_CONCAT(1, 1, 2, 3)}, state.PossiblePermutations.GetDimensions());
        for (const auto& index : Region4i(state.PossiblePermutations.GetDimensions()))
            CHECK_EQUAL(usedTransforms, state.PossiblePermutations[index]);
        for (const auto& tile : state.InputTiles)
            for (const auto& originalFace : tile.Data.Faces)
                for (auto transform : usedTransforms)
                    CHECK(state.FaceIndices.Contains(transform.ApplyToFace(originalFace)));

        //Check the link between face permutations and available tiles.
        for (int faceI = 0; faceI < N_DIRECTIONS_3D; ++faceI)
        {
            const auto& originalFace = state.InputTiles[0].Data.Faces[faceI];
            for (auto transform : usedTransforms)
            {
                auto transformedFace = transform.ApplyToFace(originalFace);
                bool expectAllPermutationToMatch = (transformedFace.Side == MinZ) ||
                                                   (transformedFace.Side == MaxZ);
                const auto& matchesToTransformedFace =
                    state.MatchingFaces[{ 0, state.FaceIndices[transformedFace] }];

                TransformSet expectedMatches;
                if (expectAllPermutationToMatch)
                    expectedMatches.Add(usedTransforms);
                else
                    expectedMatches.Add(transform);

                CHECK_EQUAL(expectedMatches, matchesToTransformedFace);
            }
        }

        //Check that the cells initialized correctly.
        CHECK_EQUAL(Vector3i{WFC_CONCAT(1, 2, 3)}, state.Cells.GetDimensions());
        for (const auto& v : Region3i(state.Cells.GetDimensions()))
        {
            const auto& cell = state.Cells[v];
            CHECK_EQUAL((TileIdx)(-1), cell.ChosenTile);
            CHECK_EQUAL(true, cell.IsChangeable);
            CHECK_EQUAL(4, cell.NPossibilities);
        }

        CHECK_EQUAL(0, state.SearchFrontier.GetSize());
        CHECK_EQUAL(0, state.UnsolvableCells.GetSize());
    }
    TEST(StateModification)
    {
        //Use two permutations of the single-tile tileset
        //    which can connect vertically.
        //Add a third permutation which doesn't line up at all with the other two,
        //    so we can test that it's never legal.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 }); // The odd one out
        State state(OneTileArmy(usedTransforms), { 4, 4, 4 });

        //Set a cell in the min corner with no transform, and check the effects.
        //Horizontally, the neighbors should be locked into using the null permutation.
        //Vertically, the neighbors could still be any permutation,
        //    as the Z face is very symmetrical.
        state.SetCell({ 0, 0, 0 }, 0, { }, true, false);
        auto* minCell = &state.Cells[{0, 0, 0}];
        //Check cell state.
        CHECK_EQUAL(false, minCell->IsChangeable);
        CHECK_EQUAL(0, minCell->ChosenTile);
        CHECK_EQUAL(Transform3D{ }, minCell->ChosenPermutation);
        CHECK_EQUAL(1, minCell->NPossibilities);
        CHECK(minCell->IsSet());
        //Check algorithm state.
        CHECK_EQUAL(0, state.UnsolvableCells.GetSize());
        CHECK_EQUAL(3, state.SearchFrontier.GetSize());
        CHECK(state.SearchFrontier.Contains({ 1, 0, 0 }));
        CHECK(state.SearchFrontier.Contains({ 0, 1, 0 }));
        CHECK(state.SearchFrontier.Contains({ 0, 0, 1 }));
        //Check the possible neighbor tiles.
        CHECK_EQUAL(TransformSet::Combine(Transform3D{ }),
                    state.PossiblePermutations[WFC_CONCAT({ 0, {1, 0, 0} })]);
        CHECK_EQUAL(TransformSet::Combine(Transform3D{ }),
                    state.PossiblePermutations[WFC_CONCAT({ 0, {0, 1, 0} })]);
        CHECK_EQUAL(TransformSet::Combine(WFC_CONCAT(
                        Transform3D{ },
                        Transform3D{ false, Rotations3D::AxisZ_90 }
                    )),
                    state.PossiblePermutations[WFC_CONCAT({ 0, { 0, 0, 1 } })]);

        //Set a second-neighbor of that cell, to make the cell in-between them unsolvable.
        state.SetCell({ 2, 0, 0 }, 0, { false, Rotations3D::AxisZ_90});
        CHECK(minCell->IsSet());
        CHECK(!state.SearchFrontier.Contains({ 1, 0, 0 }));
        CHECK_EQUAL(1, state.UnsolvableCells.GetSize());
        CHECK_EQUAL(5, state.SearchFrontier.GetSize());
        CHECK(state.SearchFrontier.Contains({ 0, 1, 0 }));
        CHECK(state.SearchFrontier.Contains({ 0, 0, 1 }));
        CHECK(state.SearchFrontier.Contains({ 3, 0, 0 }));
        CHECK(state.SearchFrontier.Contains({ 2, 1, 0 }));
        CHECK(state.SearchFrontier.Contains({ 2, 0, 1 }));
        CHECK(state.UnsolvableCells.Contains(WFC_CONCAT({ 1, 0, 0 })));
        CHECK(!state.Cells[WFC_CONCAT({ 1, 0, 0 })].IsSet());
        CHECK_EQUAL(0, state.Cells[WFC_CONCAT({ 1, 0, 0 })].NPossibilities);
        CHECK_EQUAL(TransformSet(), state.PossiblePermutations[WFC_CONCAT({ 0, { 1, 0, 0 } })]);

        //Clear the second-neighbor; it should make the immediate-neighbor solvable again.
        state.ClearCell({ 2, 0, 0 }, true);
        CHECK(minCell->IsSet());
        CHECK(!state.Cells[WFC_CONCAT({ 2, 0, 0 })].IsSet());
        CHECK(state.Cells[WFC_CONCAT({ 2, 0, 0 })].IsChangeable);
        CHECK(state.SearchFrontier.Contains({ 1, 0, 0 }));
        CHECK_EQUAL(0, state.UnsolvableCells.GetSize());
        CHECK_EQUAL(3, state.SearchFrontier.GetSize());
        CHECK(state.SearchFrontier.Contains({ 0, 1, 0 }));
        CHECK(state.SearchFrontier.Contains({ 0, 0, 1 }));
        CHECK_EQUAL(1, state.Cells[WFC_CONCAT({ 1, 0, 0 })].NPossibilities);
        CHECK_EQUAL(TransformSet::Combine(Transform3D{ }),
                    state.PossiblePermutations[WFC_CONCAT({ 0, {1, 0, 0} })]);

        //TODO: Set {1, 0, 1} to use the 2nd permutation, then {0, 0, 1} goes from 2 possibilities to 1.

        //TODO: Set a cell that isn't on the edges, and check that min faces are also updated.
    }

    //TODO: Test clearing whole, NON-SQUARE areas, including immutable cells
}


int main(int, const char* [])
{
    return UnitTest::RunAllTests();
}