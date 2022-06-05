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
        CHECK(FindBitIndex((uint32_t)0b1) == 0);
        CHECK(FindBitIndex((uint32_t)0b10) == 1);
        CHECK(FindBitIndex((uint32_t)0b100) == 2);
        CHECK(FindBitIndex((uint32_t)0b100000) == 5);
        CHECK(FindBitIndex((uint32_t)0b100000000000) == 11);
        CHECK(FindBitIndex((uint32_t)0b10000000000000000) == 16);
        CHECK(FindBitIndex((uint32_t)0b100000000000000000) == 17);
        CHECK(FindBitIndex((uint32_t)0b100000000000000000000000) == 23);
        CHECK(FindBitIndex((uint32_t)0b1000000000000000000000000000000) == 30);
        CHECK(FindBitIndex((uint32_t)0b10000000000000000000000000000000) == 31);
    }
    
    TEST(CountBitsPt1)
    {
        CHECK(CountBits((uint8_t)0b0) == 0);
        CHECK(CountBits((uint8_t)0b0110) == 2);
        CHECK(CountBits((uint8_t)0b1001) == 2);
        CHECK(CountBits((uint8_t)0b1110) == 3);
        CHECK(CountBits((uint8_t)0b01100000) == 2);
        CHECK(CountBits((uint8_t)0b10010000) == 2);
        CHECK(CountBits((uint8_t)0b11100000) == 3);
        CHECK(CountBits((uint8_t)0b10101010) == 4);
        CHECK(CountBits((uint8_t)0b11111111) == 8);

        CHECK(CountBits((uint16_t)0b0) == 0);
        CHECK(CountBits((uint16_t)0b0110) == 2);
        CHECK(CountBits((uint16_t)0b1001) == 2);
        CHECK(CountBits((uint16_t)0b1110) == 3);
        CHECK(CountBits((uint16_t)0b01100000) == 2);
        CHECK(CountBits((uint16_t)0b10010000) == 2);
        CHECK(CountBits((uint16_t)0b11100000) == 3);
        CHECK(CountBits((uint16_t)0b01100000) == 2);
        CHECK(CountBits((uint16_t)0b10010000) == 2);
        CHECK(CountBits((uint16_t)0b11100000) == 3);
        CHECK(CountBits((uint16_t)0b10101010) == 4);
        CHECK(CountBits((uint16_t)0b11111111) == 8);
        CHECK(CountBits((uint16_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint16_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint16_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint16_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint16_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint16_t)0b0110001110000000) == 5);
        CHECK(CountBits((uint16_t)0b1001000001111000) == 6);
        CHECK(CountBits((uint16_t)0b1110001000000000) == 4);
        CHECK(CountBits((uint16_t)0b0110000111111111) == 11);
        CHECK(CountBits((uint16_t)0b1001000000000001) == 3);
        CHECK(CountBits((uint16_t)0b1110010000100010) == 6);
        CHECK(CountBits((uint16_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint16_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint16_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint16_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint16_t)0b1111111111111111) == 16);
    }

    TEST(CountBitsPt2)
    {
        CHECK(CountBits((uint32_t)0b0) == 0);
        CHECK(CountBits((uint32_t)0b0110) == 2);
        CHECK(CountBits((uint32_t)0b1001) == 2);
        CHECK(CountBits((uint32_t)0b1110) == 3);
        CHECK(CountBits((uint32_t)0b01100000) == 2);
        CHECK(CountBits((uint32_t)0b10010000) == 2);
        CHECK(CountBits((uint32_t)0b11100000) == 3);
        CHECK(CountBits((uint32_t)0b01100000) == 2);
        CHECK(CountBits((uint32_t)0b10010000) == 2);
        CHECK(CountBits((uint32_t)0b11100000) == 3);
        CHECK(CountBits((uint32_t)0b10101010) == 4);
        CHECK(CountBits((uint32_t)0b11111111) == 8);
        CHECK(CountBits((uint32_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint32_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint32_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint32_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint32_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint32_t)0b0110001110000000) == 5);
        CHECK(CountBits((uint32_t)0b1001000001111000) == 6);
        CHECK(CountBits((uint32_t)0b1110001000000000) == 4);
        CHECK(CountBits((uint32_t)0b0110000111111111) == 11);
        CHECK(CountBits((uint32_t)0b1001000000000001) == 3);
        CHECK(CountBits((uint32_t)0b1110010000100010) == 6);
        CHECK(CountBits((uint32_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint32_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint32_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint32_t)0b1110010000100010) == 6);
        CHECK(CountBits((uint32_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint32_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint32_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint32_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint32_t)0b0000000011111111) == 8);
        CHECK(CountBits((uint32_t)0b1111111111111111) == 16);
    }

    TEST(CountBitsPart2_5)
    {
        CHECK(CountBits((uint32_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint32_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint32_t)0b01100011100000000000000000000000) == 5);
        CHECK(CountBits((uint32_t)0b10010000011110000000000000000000) == 6);
        CHECK(CountBits((uint32_t)0b11100010000000000000000000000000) == 4);
        CHECK(CountBits((uint32_t)0b01100001111111110000000000000000) == 11);
        CHECK(CountBits((uint32_t)0b10010000000000010000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b11100100001000100000000000000000) == 6);
        CHECK(CountBits((uint32_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint32_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint32_t)0b11100100001000100000000000000000) == 6);
        CHECK(CountBits((uint32_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint32_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint32_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint32_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint32_t)0b11111111111111110000000000000000) == 16);
        CHECK(CountBits((uint32_t)0b11100000000000000011111110000000) == 10);
        CHECK(CountBits((uint32_t)0b10101010000011100111001110011101) == 17);
        CHECK(CountBits((uint32_t)0b11111111110000110110001111000110) == 20);
        CHECK(CountBits((uint32_t)0b11111111111111110000000000000000) == 16);
        CHECK(CountBits((uint32_t)0b00000000000000001111111111111111) == 16);
        CHECK(CountBits((uint32_t)0b11111111111111111111111111111111) == 32);
    }

    TEST(CountBitsPt3)
    {
        CHECK(CountBits((uint64_t)0b0) == 0);
        CHECK(CountBits((uint64_t)0b0110) == 2);
        CHECK(CountBits((uint64_t)0b1001) == 2);
        CHECK(CountBits((uint64_t)0b1110) == 3);
        CHECK(CountBits((uint64_t)0b01100000) == 2);
        CHECK(CountBits((uint64_t)0b10010000) == 2);
        CHECK(CountBits((uint64_t)0b11100000) == 3);
        CHECK(CountBits((uint64_t)0b01100000) == 2);
        CHECK(CountBits((uint64_t)0b10010000) == 2);
        CHECK(CountBits((uint64_t)0b11100000) == 3);
        CHECK(CountBits((uint64_t)0b10101010) == 4);
        CHECK(CountBits((uint64_t)0b11111111) == 8);
        CHECK(CountBits((uint64_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint64_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint64_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint64_t)0b0110001110000000) == 5);
        CHECK(CountBits((uint64_t)0b1001000001111000) == 6);
        CHECK(CountBits((uint64_t)0b1110001000000000) == 4);
        CHECK(CountBits((uint64_t)0b0110000111111111) == 11);
        CHECK(CountBits((uint64_t)0b1001000000000001) == 3);
        CHECK(CountBits((uint64_t)0b1110010000100010) == 6);
        CHECK(CountBits((uint64_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint64_t)0b1110010000100010) == 6);
        CHECK(CountBits((uint64_t)0b0110000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000) == 8);
        CHECK(CountBits((uint64_t)0b0000000011111111) == 8);
        CHECK(CountBits((uint64_t)0b1111111111111111) == 16);
    }

    TEST(CountBitsPart3_5)
    {
        CHECK(CountBits((uint64_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b01100011100000000000000000000000) == 5);
        CHECK(CountBits((uint64_t)0b10010000011110000000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b11100010000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b01100001111111110000000000000000) == 11);
        CHECK(CountBits((uint64_t)0b10010000000000010000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b11100100001000100000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b11100100001000100000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b01100000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b10010000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b11100000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b10101010000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b11111111000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b11111111111111110000000000000000) == 16);
        CHECK(CountBits((uint64_t)0b11100000000000000011111110000000) == 10);
        CHECK(CountBits((uint64_t)0b10101010000011100111001110011101) == 17);
        CHECK(CountBits((uint64_t)0b11111111110000110110001111000110) == 20);
        CHECK(CountBits((uint64_t)0b11111111111111110000000000000000) == 16);
        CHECK(CountBits((uint64_t)0b00000000000000001111111111111111) == 16);
        CHECK(CountBits((uint64_t)0b11111111111111111111111111111111) == 32);
    }

    TEST(CountBitsPt4)
    {
        CHECK(CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b0110001110000000000000000000000000000000000000000000000000000000) == 5);
        CHECK(CountBits((uint64_t)0b1001000001111000000000000000000000000000000000000000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b1110001000000000000000000000000000000000000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b0110000111111111000000000000000000000000000000000000000000000000) == 11);
        CHECK(CountBits((uint64_t)0b1001000000000001000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1110010000100010000000000000000000000000000000000000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b1110010000100010000000000000000000000000000000000000000000000000) == 6);
        CHECK(CountBits((uint64_t)0b0110000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1001000000000000000000000000000000000000000000000000000000000000) == 2);
        CHECK(CountBits((uint64_t)0b1110000000000000000000000000000000000000000000000000000000000000) == 3);
        CHECK(CountBits((uint64_t)0b1010101000000000000000000000000000000000000000000000000000000000) == 4);
        CHECK(CountBits((uint64_t)0b1111111100000000000000000000000000000000000000000000000000000000) == 8);
        CHECK(CountBits((uint64_t)0b1111111111111111000000000000000000000000000000000000000000000000) == 16);
        CHECK(CountBits((uint64_t)0b1110000000000000001111111000000000000000000000000000000000000000) == 10);
        CHECK(CountBits((uint64_t)0b1010101000001110011100111001110100000000000000000000000000000000) == 17);
        CHECK(CountBits((uint64_t)0b1111111111000011011000111100011000000000000000000000000000000000) == 20);
        CHECK(CountBits((uint64_t)0b1111111111111111000000000000000000000000000000000000000000000000) == 16);
        CHECK(CountBits((uint64_t)0b0000000000000000111111111111111100000000000000000000000000000000) == 16);
        CHECK(CountBits((uint64_t)0b1111111111111111111111111111111100000000000000000000000000000000) == 32);
    }

    TEST(PositiveModulo)
    {
        CHECK(PositiveModulo(5, 20) == 5);
        CHECK(PositiveModulo(5, 3) == 2);
        CHECK(PositiveModulo(5, 3) == 2);
        CHECK(PositiveModulo(-5, 2) == 1);
        CHECK(PositiveModulo(-6, 2) == 0);
        CHECK(PositiveModulo(-13, 4) == 3);
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
                CHECK(inverse.ApplyToPos(forward.ApplyToPos(testPos)) == testPos);
                CHECK(forward.ApplyToPos(inverse.ApplyToPos(testPos)) == testPos);
            }
    }

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

        CHECK(set.Size() == 0);
        CHECK(CountSetIterator(set) == 0);
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        auto vec = ReadSet(set);
        CHECK(vec.size() == 0);

        set.Add(setTransforms[0]);
        CHECK(set.Size() == 1);
        CHECK(CountSetIterator(set) == 1);
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 1);
        CHECK(vec[0] == setTransforms[0]);

        set.Add(setTransforms[2]);
        CHECK(set.Size() == 2);
        CHECK(CountSetIterator(set) == 2);
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 2);
        CHECK(vec[0] == setTransforms[0]);
        CHECK(vec[1] == setTransforms[2]);

        set.Remove(setTransforms[0]);
        CHECK(set.Size() == 1);
        CHECK(CountSetIterator(set) == 1);
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 1);
        CHECK(vec[0] == setTransforms[2]);

        set.Remove(setTransforms[2]);
        CHECK(set.Size() == 0);
        CHECK(CountSetIterator(set) == 0);
        CHECK(!set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(!set.Contains(setTransforms[2]));
        CHECK(!set.Contains(setTransforms[3]));
        CHECK(!set.Contains(setTransforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 0);
    }
    TEST(TransformSetPart2)
    {
        TransformSet set;

        auto set1 = TransformSet::CombineTransforms(setTransforms[0], setTransforms[3]);
        auto set2 = TransformSet::CombineTransforms(setTransforms[2], setTransforms[4]);
        set = TransformSet::CombineTransforms(set1, set2);
        CHECK(set.Size() == 4);
        CHECK(CountSetIterator(set) == 4);
        CHECK(set.Contains(setTransforms[0]));
        CHECK(!set.Contains(setTransforms[1]));
        CHECK(set.Contains(setTransforms[2]));
        CHECK(set.Contains(setTransforms[3]));
        CHECK(set.Contains(setTransforms[4]));
        auto vec = ReadSet(set);
        CHECK(vec.size() == 4);
        CHECK(vec[0] == setTransforms[0]);
        CHECK(vec[1] == setTransforms[2]);
        CHECK(vec[2] == setTransforms[3]);
        CHECK(vec[3] == setTransforms[4]);

        CHECK(set1 != set2);
        CHECK(set == TransformSet::CombineTransforms(set1, set2));
        CHECK(set == TransformSet::CombineTransforms(set2, set1));

        set = set1;
        CHECK(set.Add(set2) == 2);
        CHECK(set == TransformSet::CombineTransforms(set1, set2));
        CHECK(set.Add(set2) == 0);
        CHECK(set == TransformSet::CombineTransforms(set1, set2));

        set = set1;
        CHECK(set.Remove(set1) == 2);
        CHECK(set == TransformSet{});
        CHECK(set.Remove(set1) == 0);
        CHECK(set == TransformSet{});

        set = TransformSet::CombineTransforms(set1, set2);
        CHECK(set.Intersect(set2) == 2);
        CHECK(set == set2);
        CHECK(set.Intersect(set2) == 0);
        CHECK(set == set2);
    }
}


int main(int, const char* [])
{
    return UnitTest::RunAllTests();
}