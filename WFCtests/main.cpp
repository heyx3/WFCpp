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
            CHECK_EQUAL(true, cell.IsChangeable);
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
        state.SetCell({ 0, 0, 0 }, 0, { }, &report, true, false);
        auto* minCell = &state.Cells[{0, 0, 0}];
        //Check cell state.
        CHECK_EQUAL(false, minCell->IsChangeable);
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
        state.SetCell({ 2, 0, 0 }, 0, { false, Rotations3D::AxisZ_90}, &report);
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
        state.ClearCell({ 2, 0, 0 }, &report, true);
        CHECK(minCell->IsSet());
        CHECK(!state.Cells[WFC_CONCAT({ 2, 0, 0 })].IsSet());
        CHECK(state.Cells[WFC_CONCAT({ 2, 0, 0 })].IsChangeable);
        CHECK(report.GotInteresting.contains({ 1, 0, 0 }));
        CHECK_EQUAL(0, report.GotUnsolvable.size());
        CHECK_EQUAL(1, report.GotInteresting.size());
        CHECK_EQUAL(1, state.Cells[WFC_CONCAT({ 1, 0, 0 })].NPossibilities);
        CHECK_EQUAL(TransformSet::Combine(Transform3D{ }),
                    state.PossiblePermutations[WFC_CONCAT({ 0, {1, 0, 0} })]);

        //Set {1, 0, 0}, so tht the next test is more interesting.
        //Don't pass the Report instance either, to make sure that use-case is fine.
        state.SetCell({ 1, 0, 0 }, 0, { });

        //Set {1, 0, 1} to use the 2nd permutation, then {0, 0, 1} goes from 2 possibilities to 1.
        Vector3i newCellPos{ 1, 0, 1 };
        Transform3D newCellTransform{ false, Rotations3D::AxisZ_90 };
        report.Clear();
        state.SetCell(newCellPos, 0, newCellTransform, &report, true, true);
        CHECK(state.Cells[newCellPos].IsSet());
        CHECK_EQUAL(0, state.Cells[newCellPos].ChosenTile);
        CHECK_EQUAL(newCellTransform, state.Cells[newCellPos].ChosenPermutation);
        CHECK(state.Cells[newCellPos].IsChangeable);
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
        Grid grid(OneTileArmy(usedTransforms), { 8, 3, 4 });
        Grid::Report report;

        //Set a bunch of cells in a region, some immutable.
        const Vector3i min(1, 2, 1),
                       max(7, 2, 3);
        const Region3i region(min, max + 1);
        const Vector3i range = region.GetSize();
        std::function<bool(Vector3i)> isMutable = [](Vector3i p) { return p.z != 2; };
        for (Vector3i p : region)
            grid.SetCell(p, 0, Transform3D{ }, nullptr, true, isMutable(p));

        //Clear all but the immutable cells.
        report.Clear();
        grid.ClearCells(region, &report);
        for (Vector3i p : region)
            CHECK_EQUAL(!isMutable(p), grid.Cells[p].IsSet());
        //The immutable Z layer means that the other Z layer stays interesting.
        for (Vector3i p : region)
            CHECK_EQUAL(!grid.Cells[p].IsSet(), report.GotInteresting.contains(p));
        CHECK(report.GotInteresting.size());

        //Make sure their neighbors were updated (except for the neighbors of immutables).
        auto checkClearedNeighbor = [&](int x, int y, int z, int dirX, int dirY, int dirZ) {
            Vector3i p = Vector3i{ x, y, z } + Vector3i{ dirX, dirY, dirZ };
            if (!grid.Cells.IsIndexValid(p))
                return;
            const auto& legalPermutations = grid.PossiblePermutations[{0, p}];
            if (isMutable({ x, y, z }))
            {
                CHECK_EQUAL(2, grid.Cells[p].NPossibilities);
                CHECK_EQUAL(usedTransforms, legalPermutations);
            }
            else
            {
                CHECK_EQUAL(1, grid.Cells[p].NPossibilities);
                CHECK_EQUAL(TransformSet::Combine(Transform3D{ }), legalPermutations);
            }
        };
        //The Z face neighbors:
        for (int y = min.y; y <= max.y; ++y)
            for (int x = min.x; x <= max.x; ++x)
                for (auto [z, dir] : std::vector{ std::make_tuple(min.z, -1),
                                                  std::make_tuple(max.z, +1) })
                    checkClearedNeighbor(x, y, z, 0, 0, dir);
        //The X- and Y-face neighbors:
        for (int z = min.z; z <= max.z; ++z)
        {
            for (int x = min.x; x <= max.x; ++x)
                for (auto [y, dir] : std::vector{ std::make_tuple(min.y, -1),
                                                  std::make_tuple(max.y, +1) })
                    checkClearedNeighbor(x, y, z, 0, dir, 0);
            for (int y = min.y; y <= max.y; ++y)
                for (auto [x, dir] : std::vector{ std::make_tuple(min.x, -1),
                                                  std::make_tuple(max.x, +1) })
                    checkClearedNeighbor(x, y, z, dir, 0, 0);
        }

        //Clear the immutable cells too.
        //Change them to mutable as they're cleared.
        report.Clear();
        grid.ClearCells(region, &report, true, true);
        for (Vector3i p : region)
        {
            CHECK(!grid.Cells[p].IsSet());
            CHECK(grid.Cells[p].IsChangeable);
            CHECK(std::find(report.GotBoring.begin(), report.GotBoring.end(), p) != report.GotBoring.end());
        }
        
        //Now the entire grid is empty.
        CHECK_EQUAL(0, report.GotInteresting.size());
        CHECK_EQUAL(0, report.GotUnsolvable.size());

        //Look for all the cells marked boring.
        std::unordered_set<Vector3i> affectedCells;
        //The immutable Z-layer was affected, along with the layers above and below it.
        for (int x = min.x; x <= max.x; ++x)
            for (int y = min.y; y <= max.y; ++y)
                for (int z = 1; z <= 3; ++z)
                    affectedCells.insert({ x, y, z});
        //The perimeter of the Z-layer was affected as well.
        for (int x = min.x; x <= max.x; ++x)
            affectedCells.insert({ x, min.y - 1, 2 });
        for (int y = min.y; y <= max.y; ++y)
            affectedCells.insert({ min.x - 1, y, 2 });
        //Now test that reality lines up with expectations.
        CHECK_EQUAL(affectedCells.size(), report.GotBoring.size());
        for (Vector3i cell : affectedCells)
            CHECK(std::find(report.GotBoring.begin(), report.GotBoring.end(), cell) != report.GotBoring.end());
        for (Vector3i cell : Region3i(Vector3i(-1, -1, -1), grid.Cells.GetDimensions() + 1))
            if (!affectedCells.contains(cell))
                CHECK(std::find(report.GotBoring.begin(), report.GotBoring.end(), cell) == report.GotBoring.end());
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

        //StandardRunner uses <random> to generate numbers.
        //Run the test many times to cover a broad range of possible outcomes.
        const int N_TESTS =
            #ifdef _DEBUG
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

        //Set one tile on each Z-slice.
        std::unordered_map<Vector3i, std::tuple<TileIdx, Transform3D>> constants;
        constants[{ 2, 2, 0 }] = std::make_tuple((TileIdx)0, Transform3D{ });
        constants[{ 2, 2, 2 }] = std::make_tuple((TileIdx)0, Transform3D{ });
        constants[{ 2, 2, 4 }] = std::make_tuple((TileIdx)0, Transform3D{ });
        constants[{ 2, 2, 1 }] = std::make_tuple((TileIdx)0, Transform3D{ false, Rotations3D::AxisZ_90 });
        constants[{ 2, 2, 3 }] = std::make_tuple((TileIdx)0, Transform3D{ false, Rotations3D::AxisZ_90 });
        constants[{ 2, 2, 5 }] = std::make_tuple((TileIdx)0, Transform3D{ false, Rotations3D::AxisZ_90 });
        state.Reset(constants);

        //Run until the whole grid is solved.
        //The number of iterations needed is simple to calculate,
        //    as the grid can only be solved one way.
        std::cout << "    (running a slow test..";
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
        //TODO: This isn't complex enough; we need it to clear sometimes
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
            , nullptr, { 0xababfefb43455501 }
        );
        state.Reset();

        //Run until the whole grid is solved.
        std::cout << "    (running a slow test..";
        bool isFinished = false;
        while (!isFinished)
        {
            isFinished = state.Tick();
        }
        std::cout << ". finished!)\n";
    }

    //Before using the SymmetricRods tileset, run a trivial test to make sure it's defined correctly.
    TEST(SymmetricRodsIsValid)
    {
        auto tileset = SymmetricRods::Create(Transform3D{ false, Rotations3D::None });

        StandardRunner state(
            tileset.Tiles, { 4, 4, 8 },
            nullptr, { 0xa33eff3456a23423 }
        );
        state.ClearRegionGrowthRateT = 0.1f;
        state.Reset();

        bool finished = state.TickN(20000);
        CHECK(finished);

        //TODO: Check the result is valid, using 'tileset.FaceGroups'.
    }
    //TODO: Test SymmetricRods while putting a constant tile C into the center, and check that it's never cleared.
}


int main(int, const char* [])
{
    std::cout << "Running tests..." << std::endl;
    return UnitTest::RunAllTests();
}