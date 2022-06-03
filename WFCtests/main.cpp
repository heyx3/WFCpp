#include "TestTilesets.hpp"

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
        CHECK(FindBitIndex(0b1) == 0);
        CHECK(FindBitIndex(0b10) == 1);
        CHECK(FindBitIndex(0b100) == 2);
        CHECK(FindBitIndex(0b100000) == 5);
        CHECK(FindBitIndex(0b100000000000) == 11);
        CHECK(FindBitIndex(0b10000000000000000) == 16);
        CHECK(FindBitIndex(0b100000000000000000) == 17);
        CHECK(FindBitIndex(0b100000000000000000000000) == 23);
        CHECK(FindBitIndex(0b1000000000000000000000000000000) == 30);
        CHECK(FindBitIndex(0b10000000000000000000000000000000) == 31);
    }
    TEST(CountBits)
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

    TEST(TransformSet)
    {
        TransformSet set;

        Transform3D transforms[] = {
            //Defined in their expected order.
            { false, Rotations3D::None },
            { false, Rotations3D::CornerBAA_120 },
            { true, Rotations3D::None },
            { true, Rotations3D::AxisZ_180 },
            { true, Rotations3D::CornerBAA_120 }
        };

        CHECK(set.Size() == 0);
        CHECK(CountSetIterator(set) == 0);
        CHECK(!set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(!set.Contains(transforms[2]));
        CHECK(!set.Contains(transforms[3]));
        CHECK(!set.Contains(transforms[4]));
        auto vec = ReadSet(set);
        CHECK(vec.size() == 0);

        set.Add(transforms[0]);
        CHECK(set.Size() == 1);
        CHECK(CountSetIterator(set) == 1);
        CHECK(set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(!set.Contains(transforms[2]));
        CHECK(!set.Contains(transforms[3]));
        CHECK(!set.Contains(transforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 1);
        CHECK(vec[0] == transforms[0]);

        set.Add(transforms[2]);
        CHECK(set.Size() == 2);
        CHECK(CountSetIterator(set) == 2);
        CHECK(set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(set.Contains(transforms[2]));
        CHECK(!set.Contains(transforms[3]));
        CHECK(!set.Contains(transforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 2);
        CHECK(vec[0] == transforms[0]);
        CHECK(vec[1] == transforms[2]);

        set.Remove(transforms[0]);
        CHECK(set.Size() == 1);
        CHECK(CountSetIterator(set) == 1);
        CHECK(!set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(set.Contains(transforms[2]));
        CHECK(!set.Contains(transforms[3]));
        CHECK(!set.Contains(transforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 1);
        CHECK(vec[0] == transforms[2]);

        set.Remove(transforms[2]);
        CHECK(set.Size() == 0);
        CHECK(CountSetIterator(set) == 0);
        CHECK(!set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(!set.Contains(transforms[2]));
        CHECK(!set.Contains(transforms[3]));
        CHECK(!set.Contains(transforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 0);

        auto set1 = TransformSet::CombineTransforms(transforms[0], transforms[3]);
        auto set2 = TransformSet::CombineTransforms(transforms[2], transforms[4]);
        set = TransformSet::CombineTransforms(set1, set2);
        CHECK(set.Size() == 4);
        CHECK(CountSetIterator(set) == 4);
        CHECK(set.Contains(transforms[0]));
        CHECK(!set.Contains(transforms[1]));
        CHECK(set.Contains(transforms[2]));
        CHECK(set.Contains(transforms[3]));
        CHECK(set.Contains(transforms[4]));
        vec = ReadSet(set);
        CHECK(vec.size() == 4);
        CHECK(vec[0] == transforms[0]);
        CHECK(vec[1] == transforms[2]);
        CHECK(vec[2] == transforms[3]);
        CHECK(vec[3] == transforms[4]);
    }
}


int main(int, const char* [])
{
    return UnitTest::RunAllTests();
}