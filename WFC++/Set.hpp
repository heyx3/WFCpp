#pragma once

#include "Platform.h"

#include <unordered_set>


namespace WFC
{
	template<typename T, typename Hasher = T>
    class Set
    {
    public:

        size_t GetSize() const { return set.size(); }

        void Reserve(size_t n) { set.reserve(n); }

        bool Add(const T& t) { return set.insert(t).second; }
        void Add(const Set<T, Hasher>& set2) { set.insert(set2.set.begin(), set2.set.end()); }

        bool Erase(const T& t) { return set.erase(t) > 0; }
        void Erase(const Set<T, Hasher>& t) { set.erase(t.begin(), t.end()); }

        bool Contains(const T& t) const { return set.find(t) != set.end(); }
        void Clear() { set.clear(); }

        //Iterators for ranged-for loops.
        //We can't use "auto" for the return type here because it breaks SWIG.
        typename std::unordered_set<T, Hasher>::const_iterator begin() const { return set.begin(); }
        typename std::unordered_set<T, Hasher>::iterator       begin()       { return set.begin(); }
        typename std::unordered_set<T, Hasher>::const_iterator end()   const { return set.end(); }
        typename std::unordered_set<T, Hasher>::iterator       end()         { return set.end(); }


    private:

        std::unordered_set<T, Hasher> set;
    };
}