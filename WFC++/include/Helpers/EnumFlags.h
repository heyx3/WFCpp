#pragma once

#include <vector>
#include <bit>
#include <optional>

namespace WFC
{
    template<typename EnumType, typename IntType, IntType EnumValEnd>
    //Efficiently represents a Set of values from a single enum.
    //Assumes they are sequentially numbered from 0 to some max value.
    struct EnumFlags
    {
        using This_t = EnumFlags<EnumType, IntType, EnumValEnd>;

        static IntType ToMask(EnumType val) { return 1 << static_cast<IntType>(val); }

        //Gets an instance where all flags are enabled.
        static This_t All() { return (IntType)(-1); }


        IntType BitMask;
        EnumFlags(IntType bitMask = 0) : BitMask(bitMask) { }
        EnumFlags(EnumType startValue) : EnumFlags(ToMask(startValue)) { }
        operator IntType() const { return BitMask; }

        bool Contains(EnumType value) const { return (BitMask & ToMask(value)) != 0; }
        int Size() const { return std::popcount(BitMask); }
        bool IsEmpty() const { return Size() == 0; }

        //Outputs all enum values this instance contains into the given list.
        template<typename Allocator>
        void Enumerate(std::vector<EnumType, Allocator>& outValues)
        {
            auto tryElement = GetFirstElement();
            while (tryElement)
            {
                outValues.push_back(*tryElement);
                tryElement = GetNextElement(*tryElement);
            }
        }

        //Iterate through the flags in this set with a call to this, followed by calls to 'GetNextElement()'.
        //As soon as null is returned, the iteration is over.
        std::optional<EnumType> GetFirstElement() const
        {
            for (IntType i = 0; i < EnumValEnd; ++i)
                if (Contains(static_cast<EnumType>(i)))
                    return static_cast<EnumType>(i);
            return std::nullopt;
        }
        //Iterate through the flags in this set with a call to 'GetFirstElement()', followed by calls to this.
        //As soon as null is returned, the iteration is over.
        std::optional<EnumType> GetNextElement(EnumType lastElement) const
        {
            for (IntType i = static_cast<IntType>(lastElement) + 1; i < EnumValEnd; ++i)
                if (Contains(static_cast<EnumType>(i)))
                    return static_cast<EnumType>(i);
            return std::nullopt;
        }

        //Operators to add/remove flags.
        EnumFlags& operator+=(EnumType toAdd) { BitMask |= ToMask(toAdd); return *this; }
        EnumFlags& operator-=(EnumType toRemove) { BitMask &= ~ToMask(toRemove); return *this; }
        EnumFlags operator+(EnumType toAdd) const { auto cpy = *this; cpy += toAdd; return cpy; }
        EnumFlags operator-(EnumType toRemove) const { auto cpy = *this; cpy -= toRemove; return cpy; }

        //Union operators:
        EnumFlags& operator|=(This_t otherFlags) { BitMask |= otherFlags.BitMask; return *this; }
        EnumFlags operator|(This_t otherFlags) const { auto cpy = *this; cpy |= otherFlags; return cpy; }

        //Intersection operators:
        EnumFlags& operator&=(This_t otherFlags) { BitMask &= otherFlags.BitMask; return *this; }
        EnumFlags operator&(This_t otherFlags) { auto cpy = *this; cpy &= otherFlags; return cpy; }

        //Comparisons (<= means "subset of" and >= means "superset of")
        bool operator==(This_t other) const { return BitMask == other.BitMask; }
        bool operator!=(This_t other) const { return BitMask != other.BitMask; }
        bool operator<=(This_t other) const { return (BitMask & other.BitMask) == BitMask; }
        bool operator>=(This_t other) const { return (BitMask & other.BitMask) == other.BitMask; }
    };
}