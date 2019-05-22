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
    }
}