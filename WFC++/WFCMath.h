#pragma once

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
    }
}