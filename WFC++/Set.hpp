#pragma once

#include "Platform.h"

#include <unordered_set>
#include <functional>


namespace WFC
{
	template<typename T, typename Hasher = T>
    class WFC_API Set
    {
    public:

        size_t GetSize() const { return set.size(); }

        void Reserve(size_t n) { set.reserve(n); }

        bool Add(const T& t) { return set.insert(t).second; }
        void Add(const Set<T, Hasher>& set2) { set.insert(set2.set.begin(), set2.set.end()) }
        bool Erase(const T& t) { return set.erase(t) > 0; }

        bool Contains(const T& t) { return set.find(t) != set.end(); }
        void Clear() { set.clear(); }

        //Iterators for ranged-for loops.
        auto begin() const { return set.begin(); }
        auto begin() { return set.begin(); }
        auto end() const { return set.end(); }
        auto end() { return set.end(); }


    private:

        std::unordered_set<T, Hasher> set;
    };

	//Use this macro to export a specific Set<> type in the dll.
    #define EXPORT_WFC_SET(elementType)                  template class WFC_API Set<elementType>;
    //Use this macro to export a specific Set<> type with the STL hasher in the dll.
    #define EXPORT_WFC_SET_STD(elementType) template class WFC_API Set<elementType, std::hash<elementType>>;
    //Use this macro to export a specific Set<> type with custom hasher in the dll.
    #define EXPORT_WFC_SET_FULL(elementType, hasherType) template class WFC_API Set<elementType, hasherType>;
}