#pragma once

#include <vector>

namespace WFC
{
    template<typename EnumType, typename IntType, IntType EnumValEnd>
    //Efficiently represents a Set of values from a single enum.
    //Assumes they are sequentially numbered from 0 to some max value.
    struct EnumFlags
    {
        using This_t = EnumFlags<EnumType, IntType, EnumValEnd>;

        static IntType ToMask(EnumType val) { return 1 << (IntType)val; }

        //Gets an instance where all flags are enabled.
        static This_t All() { return (IntType)(-1); }


        IntType BitMask;
        EnumFlags(IntType bitMask = 0) : BitMask(bitMask) { }
        EnumFlags(EnumType startValue) : EnumFlags(ToMask(startValue)) { }
        operator IntType() const { return BitMask; }

        bool Contains(EnumType value) const { return (BitMask & ToMask(value)) != 0; }

        //Outputs all enum values this instance contains into the given list.
        template<typename Allocator>
        void Enumerate(std::vector<EnumType, Allocator>& outValues)
        {
            for (IntType i = 0; i < EnumValEnd; ++i)
                if (Contains((EnumType)i))
                    outValues.push_back((EnumType)i);
        }

        //Operators to add/remove flags.
        EnumFlags& operator+=(EnumType toAdd) { BitMask |= ToMask(toAdd); return *this; }
        EnumFlags& operator-=(EnumType toRemove) { BitMask &= ~ToMask(toRemove); return *this; }
        EnumFlags operator+(EnumType toAdd) const { auto cpy = *this; cpy += toAdd; return cpy; }
        EnumFlags operator-(EnumType toAdd) const { auto cpy = *this; cpy -= toAdd; return cpy; }

        //Union operators:
        EnumFlags& operator|=(This_t otherFlags) { BitMask |= otherFlags.BitMask; return *this; }
        EnumFlags operator|(This_t otherFlags) const { auto cpy = *this; cpy |= otherFlags; return cpy; }

        //Intersection operators:
        EnumFlags& operator&=(This_t otherFlags) { BitMask &= otherFlags.BitMask; return *this; }
        EnumFlags operator&(This_t otherFlags) { auto cpy = *this; cpy &= otherFlags; return cpy; }
    };
}