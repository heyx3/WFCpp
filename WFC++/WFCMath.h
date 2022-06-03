#pragma once

#include <numeric>

namespace WFC
{
    namespace Math
    {
        //Some kind of signed integer type.
        template<typename IntType>
        //Does a more intuitive modulo operation, where the result is always positive.
        inline IntType PositiveModulo(IntType numerator, IntType denominator)
        {
            return (numerator % denominator) + (denominator * (numerator < 0));
        }

        template<typename T>
        T Min(T a, T b) { return (a < b) ? a : b; }

        template<typename T>
        T Max(T a, T b) { return (a > b) ? a : b; }

        template<typename T>
        T Clamp(T x, T min, T max) { return Min(max, Max(min, x)); }


        //Safe type-punning: reinterprets input A's byte-data as an instance of B
        //    by making a copy on the stack.
        template<typename B, typename A>
        constexpr B Reinterpret(const A& a)
        {
            static_assert(sizeof(A) >= sizeof(B),
                          "Can't Reinterpret() these two types; destination is larger than source");

            B b;
            std::memcpy(&b, &a, sizeof(B));

            return b;
        }

        //Finds which bit is set, in an integer with 1 bit set.
        inline uint_fast8_t FindBitIndex(uint32_t u)
        {
            //Accomplished by hacking on a 64-bit float.
            //Reference: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogIEEE64Float

            static_assert(std::numeric_limits<double>().is_iec559,
                          "This only works for standard 64-bit floats");
            
            //Assemble the double's bit-pattern with a specific exponent.
            const uint32_t exponent = 0x43300000;
            uint_fast8_t exponentHalf = (std::endian::native == std::endian::little) ? 1 : 0,
                         mantissaHalf = (std::endian::native == std::endian::little) ? 0 : 1;
            uint32_t bytes[2];
            bytes[exponentHalf] = exponent;
            bytes[mantissaHalf] = u;

            //Manipulate the double.
            double d;
            unsigned char* dBytes = (unsigned char*)&d;
            std::memcpy(dBytes, bytes, sizeof(double));
            d -= 4503599627370496.0;
            
            //Grab the exponent again, and use it to deduce the log
            std::memcpy(&bytes[exponentHalf],
                        &dBytes[exponentHalf * sizeof(uint32_t)],
                        sizeof(uint32_t));
            return (uint_fast8_t)((bytes[exponentHalf] >> 20) - 0x3FF);
        }

        //Counts the number of '1' bits in an integer.
        inline uint_fast8_t CountBits(uint8_t u)
        {
            //Reference: https://stackoverflow.com/questions/9949935/calculate-number-of-bits-set-in-byte
            static const uint8_t NIBBLE_LOOKUP[16] =
            {
              0, 1, 1, 2, 1, 2, 2, 3,
              1, 2, 2, 3, 2, 3, 3, 4
            };
            return NIBBLE_LOOKUP[u & 0x0F] + NIBBLE_LOOKUP[u >> 4];
        }
        inline uint_fast8_t CountBits(uint16_t u)
        {
            return CountBits((uint8_t)(u & 0xff)) +
                   CountBits((uint8_t)(u >> 8));
        }
        inline uint_fast8_t CountBits(uint32_t u)
        {
            return CountBits((uint16_t)(u & 0xffff)) +
                   CountBits((uint16_t)(u >> 16));
        }
        inline uint_fast8_t CountBits(uint64_t u)
        {
            return CountBits((uint32_t)(u & 0xffffffff)) +
                   CountBits((uint32_t)(u >> 32));
        }


        template<size_t NBits>
        inline constexpr auto impl_StorageTypeValue()
        {
            if constexpr (NBits <= 8) { return uint_least8_t{}; }
            else if constexpr (NBits <= 16) { return uint_least16_t{}; }
            else if constexpr (NBits <= 32) { return uint_least32_t{}; }
            else if constexpr (NBits <= 64) { return uint_least64_t{}; }
            else { static_assert(NBits <= 64, "Bit size is too big!"); return uint64_t{}; }
        }
        template<size_t NBits>
        struct impl_SmallestUInt { using value = decltype(impl_StorageTypeValue<NBits>()); };

        //Finds an integer type that has at least N bits.
        template<size_t NBits>
        using SmallestUInt = typename impl_SmallestUInt<NBits>::value;
    }
}