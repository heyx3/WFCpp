#pragma once

#include "Platform.h"

#include <vector>


namespace WFC
{
    //TODO: Rename 'Array1D', switch all these collections to just use vector and unordered_map (and fix hashing!)
	template<typename T>
    class List
    {
    public:

        List() { }

        explicit List(int size) : vec(size) { }
        explicit List(const T& first) { vec.push_back(first); }
        explicit List(const T& first, const T& second) { vec.push_back(first); vec.push_back(second); }
        explicit List(const T& first, const T& second, const T& third)
            { vec.push_back(first); vec.push_back(second); vec.push_back(third); }
        explicit List(const T& first, const T& second, const T& third, const T& fourth)
            { vec.push_back(first); vec.push_back(second); vec.push_back(third); vec.push_back(fourth); }
        List(std::initializer_list<T> list) : vec(list) { }

        //Move operators:
        List(List<T>&& from) : vec(std::move(from.vec)) { }
        List<T>& operator=(List<T>&& from)
        {
            vec = std::move(from.vec);
            return *this;
        }
        //Copy operators:
        List(const List<T>& from) : vec(from.vec) { }
        List<T>& operator=(const List<T>& from)
        {
            vec = from.vec;
            return *this;
        }


        size_t GetSize() const { return vec.size(); }

        T* GetData() { return vec.data(); }
        const T* GetData() const { return vec.data(); }

        //Sets this vector to increase/decrease its size to the given value.
        void Resize(size_t n) { vec.resize(n); }
        //Reserves at least the given amount of space in this vector.
        void Reserve(size_t n) { vec.reserve(n); }

        //Adds the given value to the end of this vector.
        void PushBack(const T& value) { vec.push_back(value); }
        //Adds the given value to the beginning of this vector.
        void PushFront(const T& value) { Insert(0, value); }

        T PopBack() { T val = std::move(vec.back()); vec.pop_back(); return val; }
        T PopFront() { T val = std::move(vec[0]); vec.erase(vec.begin()); return val; }
        void RemoveAt(size_t i) { vec.erase(vec.begin() + i); }

        void Insert(size_t i, const T& value) { vec.insert(vec.begin() + i, value); }
        void Insert(size_t i, const List<T>& elements) { vec.insert(vec.begin() + i, elements.begin(), elements.end()); }

        void Clear() { vec.clear(); }

        template<typename Predicate>
        int IndexOf(Predicate p) const
        {
            for (size_t i = 0; i < GetSize(); ++i)
                if (p(vec[i]))
                    return (int)i;
            return -1;
        }

        T& operator[](size_t i) { return vec[i]; }
        const T& operator[](size_t i) const { return vec[i]; }

        T& GetBack() { return vec[vec.size() - 1]; }
        const T& GetBack() const { return vec[vec.size() - 1]; }

        //Iterators for ranged-for loops.
        //We can't use "auto" for the return type here because it breaks SWIG.
        typename std::vector<T>::const_iterator begin() const { return vec.begin(); }
        typename std::vector<T>::iterator       begin()       { return vec.begin(); }
        typename std::vector<T>::const_iterator end()   const { return vec.end(); }
        typename std::vector<T>::iterator       end()         { return vec.end(); }

    private:

        std::vector<T> vec;
    };
}