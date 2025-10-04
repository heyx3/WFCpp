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
    
    TEST(CountBitsU8)
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
    }
    TEST(CountBitsU16)
    {
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
    TEST(CountBitsU32Pt1)
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
    TEST(CountBitsU32Pt2)
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
    TEST(CountBitsU64Pt1)
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
    TEST(CountBitsU64Pt2)
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
    TEST(CountBitsU64Pt3)
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

    TEST(TransformCombinations)
    {
        Transform3D identity{ };

        //Do some comprehensive tautological tests on every possible transform.
        for (int rotI = 0; rotI < N_ROTATIONS_3D; ++rotI)
        {
            auto rot = (Rotations3D)rotI;

            //Check that inverting then rotating is the same as inverting+rotating.
            CHECK_EQUAL((Transform3D{ true, Rotations3D::None }.Then(Transform3D{ false, rot })),
                (Transform3D{ true, rot }));

            for (int invert = 0; invert < 2; ++invert)
            {
                Transform3D forward = { invert == 1, rot };

                //Check that combining this transform and its inverse always leads to the identity.
                auto inverse = forward.Inverse();
                CHECK_EQUAL(forward.Then(inverse), identity);
                CHECK_EQUAL(inverse.Then(forward), identity);

                //Check that combining this transform with 4 90-degree rotations
                //     always leads to the original transform again.
                for (auto rot90 : std::to_array({ Transform3D{ false, Rotations3D::AxisX_90 },
                                                  Transform3D{ false, Rotations3D::AxisY_90 },
                                                  Transform3D{ false, Rotations3D::AxisZ_90 } }))
                {
                    CHECK_EQUAL(forward.Then(rot90).Then(rot90).Then(rot90).Then(rot90), forward);
                    CHECK_EQUAL(identity.Then(rot90).Then(rot90).Then(rot90).Then(rot90).Then(forward), forward);

                    auto rot270 = rot90.Inverse();
                    CHECK_EQUAL(forward.Then(rot90).Then(rot270), forward);
                    CHECK_EQUAL(identity.Then(rot90).Then(rot270).Then(forward), forward);
                }
                //Check that combining this transform with 3 120-degree rotations
                //     always leads to the original transform again.
                for (auto rot120 : std::to_array({ Transform3D{ false, Rotations3D::CornerAAA_120 },
                                                   Transform3D{ false, Rotations3D::CornerABA_120 },
                                                   Transform3D{ false, Rotations3D::CornerBAA_120 },
                                                   Transform3D{ false, Rotations3D::CornerBBA_120 } }))
                {
                    CHECK_EQUAL(forward.Then(rot120).Then(rot120).Then(rot120), forward);
                    CHECK_EQUAL(identity.Then(rot120).Then(rot120).Then(rot120).Then(forward), forward);

                    auto rot240 = rot120.Inverse();
                    CHECK_EQUAL(forward.Then(rot120).Then(rot240), forward);
                    CHECK_EQUAL(identity.Then(rot120).Then(rot240).Then(forward), forward);
                    CHECK_EQUAL(forward.Then(rot240).Then(rot120), forward);
                    CHECK_EQUAL(identity.Then(rot240).Then(rot120).Then(forward), forward);
                }
                //Check that combining this transform with 2 180-degree rotations
                //     always leads to the original transform again.
                for (auto rot180 : std::to_array({ Transform3D{ false, Rotations3D::AxisX_180 },
                                                   Transform3D{ false, Rotations3D::AxisY_180 },
                                                   Transform3D{ false, Rotations3D::AxisZ_180 },
                                                   Transform3D{ false, Rotations3D::EdgesXa },
                                                   Transform3D{ false, Rotations3D::EdgesXb },
                                                   Transform3D{ false, Rotations3D::EdgesYa },
                                                   Transform3D{ false, Rotations3D::EdgesYb },
                                                   Transform3D{ false, Rotations3D::EdgesZa },
                                                   Transform3D{ false, Rotations3D::EdgesZb } }))
                {
                    CHECK_EQUAL(forward.Then(rot180).Then(rot180), forward);
                    CHECK_EQUAL(identity.Then(rot180).Then(rot180).Then(forward), forward);
                }
            }
        }

        //Check specific cases that I worked out in my head.
        CHECK_EQUAL((Transform3D{ false, Rotations3D::AxisX_90 }
                      .Then(Transform3D{ false, Rotations3D::AxisX_180 })),
                    (Transform3D{ false, Rotations3D::AxisX_270 }));
        //TODO: Come up with some complex ones involving inversion; use Blender to visualize
    }

    // NOTE: Tests of Tiled3D::GetFace() would be nice,
    //     but the asserts practically make it self-testing so it's not high-priority.

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

    TEST(ImplicitTransformSet)
    {
        ImplicitTransformSet s1;
        s1.AllowAllRotations = true;
        s1.AllowInversion = true;
        CHECK_EQUAL(N_TRANSFORMS, s1.GetExplicit().Size());

        ImplicitTransformSet s5;
        s5.InitialTransforms.Add(Transform3D{ true, Rotations3D::None });
        s5.AllowAllRotations = true;
        CHECK_EQUAL(N_TRANSFORMS, s5.GetExplicit().Size());

        ImplicitTransformSet s2;
        s2.AllowAxisZRots = true;
        CHECK_EQUAL(4, s2.GetExplicit().Size());
        CHECK(s2.GetExplicit().Contains(Transform3D{ false, Rotations3D::None }));
        CHECK(s2.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_90 }));
        CHECK(s2.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_180 }));
        CHECK(s2.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_270 }));

        ImplicitTransformSet s3;
        s3.InitialTransforms.Add(Transform3D{ true, Rotations3D::None });
        s3.AllowAxisZRots = true;
        CHECK_EQUAL(8, s3.GetExplicit().Size());
        CHECK(s3.GetExplicit().Contains(Transform3D{ false, Rotations3D::None }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_90 }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_180 }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_270 }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ true, Rotations3D::None }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ true, Rotations3D::AxisZ_90 }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ true, Rotations3D::AxisZ_180 }));
        CHECK(s3.GetExplicit().Contains(Transform3D{ true, Rotations3D::AxisZ_270 }));

        ImplicitTransformSet s4;
        s4.InitialTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        CHECK_EQUAL(2, s4.GetExplicit().Size());
        CHECK(s4.GetExplicit().Contains(Transform3D{ false, Rotations3D::None }));
        CHECK(s4.GetExplicit().Contains(Transform3D{ false, Rotations3D::AxisZ_90 }));

        //TODO: More tests (I did all corner rotations but it also led to all axis-180 rotations and idk if that's correct).
    }

    TEST(GridInitialization)
    {
        //Use inverted and/or sideways permutations of the single-tile tileset.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.AddInvertedVersions();
        //TODO: Add one permutation that doesn't affect the X faces.
        Grid grid(OneTileArmy(usedTransforms), { 1, 2, 3 });

        CHECK_EQUAL(1, grid.InputTiles.size());
        CHECK_EQUAL(4, grid.NPermutedTiles);

        //The Z faces are entirely symmetrical, so the rotation/inversion
        //    should not change them.
        //The same does not hold for the X or Y faces.
        CHECK_EQUAL(2 + (4 * 4), grid.GetFaceIndices().size());

        //Check that the input tile data transferred over correctly.
        CHECK_EQUAL(Vector4i{WFC_CONCAT(1, 1, 2, 3)}, grid.PossiblePermutations.GetDimensions());
        for (const auto& index : Region4i(grid.PossiblePermutations.GetDimensions()))
            CHECK_EQUAL(usedTransforms, grid.PossiblePermutations[index]);
        for (const auto& tile : grid.InputTiles)
            for (const auto& originalFace : tile.Data.Faces)
                for (auto transform : usedTransforms)
                    CHECK(grid.GetFaceIndices().contains(transform.ApplyToFace(originalFace)));

        //Check the link between face permutations and available tiles.
        for (int faceI = 0; faceI < N_DIRECTIONS_3D; ++faceI)
        {
            const auto& originalFace = grid.InputTiles[0].Data.Faces[faceI];
            for (auto transform : usedTransforms)
            {
                auto transformedFace = transform.ApplyToFace(originalFace);
                bool expectAllPermutationToMatch = (transformedFace.Side == MinZ) ||
                                                   (transformedFace.Side == MaxZ);
                const auto& matchesToTransformedFace =
                    grid.GetMatchingFaces()[{ 0, grid.GetFaceIndices().at(transformedFace) }];

                TransformSet expectedMatches;
                if (expectAllPermutationToMatch)
                    expectedMatches.Add(usedTransforms);
                else
                    expectedMatches.Add(transform);

                CHECK_EQUAL(expectedMatches, matchesToTransformedFace);
            }
        }

        //Check that the cells initialized correctly.
        CHECK_EQUAL(Vector3i{WFC_CONCAT(1, 2, 3)}, grid.Cells.GetDimensions());
        for (const auto& v : Region3i(grid.Cells.GetDimensions()))
        {
            const auto& cell = grid.Cells[v];
            CHECK_EQUAL((TileIdx)(-1), cell.ChosenTile);
            CHECK_EQUAL(4, cell.NPossibilities);
        }
    }
    TEST(GridModification)
    {
        //Use two permutations of the single-tile tileset
        //    which can connect vertically.
        //Add a third permutation which doesn't line up at all with the other two,
        //    so we can test that it's never legal.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 }); // The odd one out
        Grid state(OneTileArmy(usedTransforms), { 4, 4, 4 });
        Grid::Report report;

        //Set a cell in the min corner with the identity transform, and check the effects.
        //Horizontally, the neighbors should be locked into using the same permutation.
        //Vertically, the neighbors could still be any permutation,
        //    as the Z face is very symmetrical.
        state.SetCell({ 0, 0, 0 }, 0, { }, false, &report, true);
        auto* minCell = &state.Cells[{0, 0, 0}];
        //Check cell state.
        CHECK_EQUAL(0, minCell->ChosenTile);
        CHECK_EQUAL(Transform3D{ }, minCell->ChosenPermutation);
        CHECK_EQUAL(1, minCell->NPossibilities);
        CHECK(minCell->IsSet());
        //Check algorithm state.
        CHECK_EQUAL(0, report.GotUnsolvable.size());
        CHECK_EQUAL(3, report.GotInteresting.size());
        CHECK(report.GotInteresting.contains({ 1, 0, 0 }));
        CHECK(report.GotInteresting.contains({ 0, 1, 0 }));
        CHECK(report.GotInteresting.contains({ 0, 0, 1 }));
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
        report.Clear();
        state.SetCell({ 2, 0, 0 }, 0, { false, Rotations3D::AxisZ_90}, false, &report);
        CHECK(minCell->IsSet());
        CHECK_EQUAL(1, report.GotUnsolvable.size());
        CHECK_EQUAL(3, report.GotInteresting.size());
        CHECK(report.GotInteresting.contains({ 3, 0, 0 }));
        CHECK(report.GotInteresting.contains({ 2, 1, 0 }));
        CHECK(report.GotInteresting.contains({ 2, 0, 1 }));
        CHECK(report.GotUnsolvable.contains(WFC_CONCAT({ 1, 0, 0 })));
        CHECK(!state.Cells[WFC_CONCAT({ 1, 0, 0 })].IsSet());
        CHECK_EQUAL(0, state.Cells[WFC_CONCAT({ 1, 0, 0 })].NPossibilities);
        CHECK_EQUAL(TransformSet(), state.PossiblePermutations[WFC_CONCAT({ 0, { 1, 0, 0 } })]);

        //Clear the second-neighbor; it should make the immediate-neighbor solvable again.
        report.Clear();
        state.ClearCell({ 2, 0, 0 }, &report);
        CHECK(minCell->IsSet());
        CHECK(!state.Cells[WFC_CONCAT({ 2, 0, 0 })].IsSet());
        CHECK_EQUAL(0, report.GotUnsolvable.size());
        CHECK_EQUAL(1, report.GotInteresting.size());
        CHECK(report.GotInteresting.contains({ 2, 0, 0 }));
        CHECK_EQUAL(1, state.Cells[WFC_CONCAT({ 1, 0, 0 })].NPossibilities);
        CHECK_EQUAL(TransformSet::Combine(Transform3D{ }),
                    state.PossiblePermutations[WFC_CONCAT({ 0, {1, 0, 0} })]);

        //Set {1, 0, 0}, so that the next test is more interesting.
        //Don't pass the Report instance either, to test that that use-case is fine.
        state.SetCell({ 1, 0, 0 }, 0, { }, true);

        //Set {1, 0, 1} to use the 2nd permutation, then {0, 0, 1} goes from 2 possibilities to 1.
        Vector3i newCellPos{ 1, 0, 1 };
        Transform3D newCellTransform{ false, Rotations3D::AxisZ_90 };
        report.Clear();
        state.SetCell(newCellPos, 0, newCellTransform, true, &report, true);
        CHECK(state.Cells[newCellPos].IsSet());
        CHECK_EQUAL(0, state.Cells[newCellPos].ChosenTile);
        CHECK_EQUAL(newCellTransform, state.Cells[newCellPos].ChosenPermutation);
        CHECK(report.GotInteresting.contains({ 2, 0, 1 }));
        CHECK(report.GotInteresting.contains({ 1, 1, 1 }));
        CHECK(report.GotInteresting.contains({ 1, 0, 2 }));
        CHECK(!report.GotInteresting.contains({ 1, 0, 0 })); // Because it's already set
        CHECK(report.GotInteresting.contains({ 0, 0, 1 }));
        CHECK_EQUAL(4, report.GotInteresting.size());
        CHECK_EQUAL(1, state.Cells[newCellPos.LessX()].NPossibilities);
    }
    TEST(GridClearing)
    {
        //Use one tile and two incompatible permutations.
        //Use a non-square grid size for more interesting tests.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 });
        Grid grid(OneTileArmy(usedTransforms), { 8, 3, 6 });
        Grid::Report report;

        //Set a bunch of cells in a region, some immutable.
        const Vector3i min(1, 2, 1),
                       max(7, 2, 4);
        const Region3i region(min, max + 1),
                       inflatedRegion(region.MinInclusive - 1, region.MaxExclusive + 1);
        const Vector3i range = region.GetSize();
        std::unordered_set<Vector3i> permanentCells;
        std::function<bool(Vector3i)> isPermanent = [](Vector3i p) { return p.z != 2; };
        for (Vector3i p : region)
        {
            grid.SetCell(p, 0, Transform3D{ }, isPermanent(p), nullptr, true);
            if (isPermanent(p))
                permanentCells.insert(p);
        }
        //Also track the cells neighboring permanent/nonpermanent.
        //Permanent-neighbors will have a permanent constraint on the face bordering that neighbor.
        std::unordered_set<Vector3i> neighborsOfPermanentCells, neighborsOfNormalClearedCells;
        for (Vector3i p : inflatedRegion)
        {
            if (grid.Cells.IsIndexValid(p) && !permanentCells.contains(p))
            {
                auto neighbors = p.OrthogonalNeighbors();
                if (std::any_of(neighbors.begin(), neighbors.end(), [&](Vector3i n) { return permanentCells.contains(n); }))
                    neighborsOfPermanentCells.insert(p);
                else if (!region.Contains(p) && std::any_of(neighbors.begin(), neighbors.end(), [&](Vector3i n) { return region.Contains(n); }))
                    neighborsOfNormalClearedCells.insert(p);
            }
        }

        //Clear all cells; the 'permanent' ones and their neighbors should remain fixed
        //     (though not officially set).
        report.Clear();
        grid.ClearCells(region, &report);
        for (Vector3i p : inflatedRegion)
            if (grid.Cells.IsIndexValid(p))
            {
                CHECK_EQUAL((permanentCells.contains(p) || neighborsOfPermanentCells.contains(p)) ? 1 : usedTransforms.Size(),
                            grid.Cells[p].NPossibilities);
            }
        //Permanent cells and their neighbors are still interesting, but all the rest got boring.
        CHECK_EQUAL(neighborsOfNormalClearedCells.size(), report.GotBoring.size());
        CHECK_EQUAL(0, report.GotInteresting.size());
        CHECK_EQUAL(0, report.GotUnsolvable.size());
    }

    TEST(StandardRunnerTick)
    {
        //Use two permutations of a single tile,
        //    which are compatible along Z but not X or Y,
        //    ensuring that each Z-level uses all one permutation.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        StandardRunner state(OneTileArmy(usedTransforms), {4, 5, 6});

        //Disable randomness in the standard-runner.
        state.PriorityWeightRandomness = 0;

        //StandardRunner uses <random> to generate numbers.
        //Run the test many times to cover a broad range of possible outcomes.
        const int N_TESTS =
            #ifdef _DEBUG
                (sizeof(size_t) == 4) ? //Very slow on 32-bit for some reason
                    5000 :
                    20000
            #else
                200000
            #endif
        ;
        for (int i = 0; i < N_TESTS; ++i)
        {
            state.Reset();

            //Tick once, and check that one cell is set.
            bool isFinished = state.Tick();
            CHECK(!isFinished);
            int nSet = 0;
            Vector3i setCellPos;
            for (Vector3i cellPos : Region3i(state.Grid.Cells.GetDimensions()))
            {
                const auto& cell = state.Grid.Cells[cellPos];
                if (cell.IsSet())
                {
                    nSet += 1;
                    setCellPos = cellPos;
                    CHECK_EQUAL(0, cell.ChosenTile);
                    CHECK(cell.ChosenPermutation ==
                            Transform3D{ } ||
                          cell.ChosenPermutation ==
                            Transform3D{WFC_CONCAT(false, Rotations3D::AxisZ_90)});
                }
            }
            CHECK_EQUAL(1, nSet);

            //Tick again, and check that the new set cell is a neighbor of the previous one.
            isFinished = state.Tick();
            CHECK(!isFinished);
            Vector3i originalCellPos = setCellPos;
            nSet = 0;
            for (Vector3i cellPos : Region3i(state.Grid.Cells.GetDimensions()))
            {
                const auto& cell = state.Grid.Cells[cellPos];
                if (cell.IsSet())
                {
                    nSet += 1;
                    CHECK_EQUAL(0, cell.ChosenTile);

                    REQUIRE CHECK(cell.ChosenPermutation ==
                          state.Grid.Cells[originalCellPos].ChosenPermutation);

                    if (cellPos != originalCellPos)
                        setCellPos = cellPos;
                }
            }
            CHECK_EQUAL(2, nSet);
            CHECK(setCellPos == originalCellPos.LessX() ||
                    setCellPos == originalCellPos.MoreX() ||
                    setCellPos == originalCellPos.LessY() ||
                    setCellPos == originalCellPos.MoreY() ||
                    setCellPos == originalCellPos.LessZ() ||
                    setCellPos == originalCellPos.MoreZ());

            //TODO: Clear an area, set a cell that makes the grid unsolvable, then test that another clear happens soon. Finally, test that it eventually completes the grid after clearing.
        }
    }

    //Before using the SymmetricRods tileset, run a trivial test to make sure it's defined correctly.
    TEST(SymmetricRodsIsValid)
    {
        auto tileset = SymmetricRods::Create(Transform3D{ false, Rotations3D::None });

        StandardRunner state(
            tileset.Tiles, { 4, 4, 8 },
            { 0xa33eff3456a23423 }
        );
        state.ClearRegionGrowthRateT = 0.001f;
        state.PriorityWeightRandomness = 0;
        state.Reset();

        bool finished = state.TickN(state.Grid.Cells.GetNumbElements() * 2000);
        CHECK(finished);

        //TODO: Check the result is valid, using 'tileset.FaceGroups'.
    }

    TEST(StandardRunnerTickN)
    {
        //Use two permutations of a single tile,
        //    which are compatible along Z but not X or Y,
        //    ensuring that each Z-level uses all one permutation.
        //Add a third incompatible permutation just to make sure it never appears.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 }); //The incompatible one.
        //Use a really big world grid to do some stress- and performance-testing.
        StandardRunner state(OneTileArmy(usedTransforms),
            #ifdef _DEBUG
                { 10, 10, 10 }
            #else
                { 50, 75, 100 }
            #endif
        );

        //Disable randomness in the standard-runner.
        state.PriorityWeightRandomness = 0;

        //Set one tile on each Z-slice.
        state.SetCell({ 2, 2, 0 }, 0, Transform3D{ });
        state.SetCell({ 2, 2, 2 }, 0, Transform3D{ });
        state.SetCell({ 2, 2, 4 }, 0, Transform3D{ });
        state.SetCell({ 2, 2, 1 }, 0, Transform3D{ false, Rotations3D::AxisZ_90 });
        state.SetCell({ 2, 2, 3 }, 0, Transform3D{ false, Rotations3D::AxisZ_90 });
        state.SetCell({ 2, 2, 5 }, 0, Transform3D{ false, Rotations3D::AxisZ_90 });

        //Run until the whole grid is solved.
        //The number of iterations needed is simple to calculate,
        //    as the grid can only be solved one way.
        std::cout << "    (running slow test 1..";
        int nLeft = state.Grid.Cells.GetNumbElements() - 6;
        for (int i = 0; i < nLeft - 1; ++i)
        {
            auto didEnd = state.Tick();
            CHECK(!didEnd);
        }
        std::cout << ". finished!)\n";
        //Finish the last tile.
        auto didEnd = state.Tick();
        CHECK(!didEnd);
        //Tick once more to detect that the algorithm is finished.
        didEnd = state.Tick();
        CHECK(didEnd);

        //Check the chosen tiles on each Z-slice.
        for (int z = 0; z < state.Grid.Cells.GetDepth(); ++z)
        {
            const auto& expectedCell = state.Grid.Cells[{ 2, 2, z }];
            for (int y = 0; y < state.Grid.Cells.GetHeight(); ++y)
            {
                for (int x = 0; x < state.Grid.Cells.GetWidth(); ++x)
                {
                    const auto& cell = state.Grid.Cells[{ x, y, z }];
                    CHECK(cell.IsSet());
                    CHECK_EQUAL(expectedCell.ChosenTile, cell.ChosenTile);
                    CHECK_EQUAL(expectedCell.ChosenPermutation, cell.ChosenPermutation);
                }
            }
        }
    }

    TEST(StandardRunnerTricky1)
    {
        //Use a large world grid and tile-set to do more thorough tests.
        //TODO: This isn't complex enough; we need a tileset that may lead to grid clearing without ruining a whole Z-slice
        StandardRunner state(
            OneTileArmy(TransformSet::Combine(
                Transform3D{ false, Rotations3D::None },
                Transform3D{ false, Rotations3D::AxisY_90 },
                Transform3D{ false, Rotations3D::AxisZ_180},
                Transform3D{ false, Rotations3D::AxisY_270 },

                Transform3D{ true, Rotations3D::None },
                Transform3D{ true, Rotations3D::AxisY_90 },
                Transform3D{ true, Rotations3D::AxisZ_180 },
                Transform3D{ true, Rotations3D::AxisY_270 },

                Transform3D{ false, Rotations3D::CornerAAA_120 },
                Transform3D{ false, Rotations3D::CornerAAA_240 }
            )),
            #ifdef _DEBUG
                { 4, 4, 100 }
            #else
                { 50, 75, 100 }
            #endif
            , { 0xababfefb43455501 }
        );
        state.Reset();

        //Disable randomness in the standard-runner.
        state.PriorityWeightRandomness = 0;

        //Run until the whole grid is solved.
        std::cout << "    (running slow test 2..";
        bool isFinished = false;
        while (!isFinished)
        {
            isFinished = state.Tick();
        }
        std::cout << ". finished!)\n";
    }

    TEST(GridConstraints)
    {
        //Use two permutations of the single-tile tileset
        //    which can connect vertically.
        //Add a third permutation which doesn't line up *at all* with the other two.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 }); // The odd one out
        Grid state(OneTileArmy(usedTransforms), { 4, 4, 4 });
        Grid::Report report;

        //Set a cell in the min corner with the identity transform.
        //Horizontally, the neighbors should be locked into using the same permutation.
        //Vertically, the neighbors could still be either permutation,
        //    as the Z face is very symmetrical.
        state.SetCell({ 0, 0, 0 }, 0, { }, false, nullptr, true);

        //Set a face constraint on the second Z level that is consistent with the second permutation.
        //This horizontal slice must now use that permutation only.
        state.SetFace({ 1, 1, 1 }, WFC::Tiled3D::MaxX,
                      state.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxY].Points,
                      &report);
        CHECK(report.GotInteresting.contains({ 1, 1, 1 }));
        CHECK(report.GotInteresting.contains({ 2, 1, 1 }));
        CHECK_EQUAL(2, report.GotInteresting.size());
        CHECK_EQUAL(0, report.GotBoring.size());
        CHECK_EQUAL(0, report.GotUnsolvable.size());
        report.Clear();

        //Set a face not-constraint on the third Z level to forbid the second permutation.
        state.SetFaceNot({ 2, 2, 2 }, WFC::Tiled3D::MaxX,
                         state.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxY].Points,
                         &report);
        CHECK(report.GotInteresting.contains({ 2, 2, 2 }));
        CHECK(report.GotInteresting.contains({ 3, 2, 2 }));
        CHECK_EQUAL(0, report.GotBoring.size());
        CHECK_EQUAL(0, report.GotUnsolvable.size());
        report.Clear();

        //Add more FaceNot constraints to make the third Z level unsolvable.
        state.SetFaceNot({ 2, 2, 2 }, WFC::Tiled3D::MaxX,
                         state.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxX].Points);
        state.SetFace({ 2, 2, 2 }, WFC::Tiled3D::MaxZ,
                      state.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxZ].Points,
                      &report);
        CHECK(report.GotUnsolvable.contains({ 2, 2, 2 }));
        CHECK_EQUAL(1, report.GotUnsolvable.size());
        CHECK(report.GotInteresting.contains({ 2, 2, 3 }));
        CHECK_EQUAL(1, report.GotInteresting.size());
        CHECK_EQUAL(0, report.GotBoring.size());
    }
    TEST(GridConstraints2)
    {
        //Use two permutations of the single-tile tileset
        //    which can connect vertically.
        //Add a third permutation which doesn't line up *at all* with the other two.
        TransformSet usedTransforms;
        usedTransforms.Add(Transform3D{ });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisZ_90 });
        usedTransforms.Add(Transform3D{ false, Rotations3D::AxisY_90 }); // The odd one out
        StandardRunner state(OneTileArmy(usedTransforms), { 4, 4, 4 });

        //Set a cell in the min corner with the identity transform.
        //Horizontally, the neighbors should be locked into using the same permutation.
        //Vertically, the neighbors could still be either permutation,
        //    as the Z face is very symmetrical.
        state.SetCell({ 0, 0, 0 }, 0, { }, false);

        //Set a face constraint on the second Z level that is consistent with the second permutation.
        //This horizontal slice must now use that permutation only.
        state.SetFaceConstraint({ 1, 1, 1 }, WFC::Tiled3D::MaxX,
                                state.Grid.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxY].Points);

        //Set a face not-constraint on the third Z level to forbid the second permutation.
        state.SetFaceConstraintNot({ 2, 2, 2 }, WFC::Tiled3D::MaxX,
                                   state.Grid.InputTiles[0].Data.Faces[WFC::Tiled3D::MaxY].Points);

        //In the second test, run the solver and check that we get the expected permutations on the first three Z levels.
        bool finished = state.TickN(20000);
        CHECK(finished);
        for (int x = 0; x < state.Grid.Cells.GetWidth(); ++x)
            for (int y = 0; y < state.Grid.Cells.GetHeight(); ++y)
            {
                CHECK_EQUAL(Transform3D{ },
                            state.Grid.Cells[Vector3i(x, y, 0)].ChosenPermutation);
                CHECK_EQUAL(Transform3D(false, Rotations3D::AxisZ_90),
                            state.Grid.Cells[Vector3i(x, y, 1)].ChosenPermutation);
                CHECK_EQUAL(Transform3D{ },
                            state.Grid.Cells[Vector3i(x, y, 2)].ChosenPermutation);
            }
    }

    //TODO: Test SymmetricRods while putting a constant tile C into the center, and check that it's never cleared.
}


int main(int, const char* [])
{
    std::cout << "Running tests..." << std::endl;
    return UnitTest::RunAllTests();
}