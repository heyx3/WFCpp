#pragma once

#include "Platform.h"

#include <unordered_map>


namespace WFC
{
	template<typename TKey, typename TValue, typename Hasher = TKey>
	//A wrapper around std::unordered_map so that it can cross DLL boundaries.
	class Dictionary
	{
	public:

		size_t GetSize() const { return dict.size(); }
		void Reserve(size_t n) { dict.reserve(n); }

		bool Contains(const TKey& key) const { return dict.find(key) != dict.end(); }
		void Clear() { dict.clear(); }

		TValue& operator[](const TKey& key) { return dict[key]; }
		const TValue& operator[](const TKey& key) const { return dict.at(key); }

		const TValue* TryGet(const TKey& key) const
		{
			auto found = dict.find(key);
			if (found == dict.end())
				return nullptr;
			else
				return &found->second;
		}
		TValue* TryGet(const TKey& key)
		{
			return (TValue*)((const Dictionary<TKey, TValue, Hasher>*)this)->TryGet(key);
		}

		const TValue& Get(const TKey& key, const TValue& valIfNotFound) const
		{
			auto tried = TryGet(key);
			return (tried == nullptr ?
				valIfNotFound :
				*tried);
		}
		TValue& Get(const TKey& key, const TValue& valIfNotFound)
		{
			return (TValue&)((const Dictionary<TKey, TValue, Hasher>*)this)->Get(key, valIfNotFound);
		}

        template<typename NumberType>
        //Sums the values.
        NumberType Sum(NumberType(*valueToNumber)(const TValue& value)) const
        {
            NumberType sum = 0;
            for (auto& keyAndVal : dict)
                sum += valueToNumber(keyAndVal.second);
            return sum;
        }

        //Iterators for ranged-for loops.
        //We can't use "auto" for the return type here because it breaks SWIG.
        typename std::unordered_map<TKey, TValue, Hasher>::const_iterator begin() const { return dict.begin(); }
        typename std::unordered_map<TKey, TValue, Hasher>::iterator       begin()       { return dict.begin(); }
        typename std::unordered_map<TKey, TValue, Hasher>::const_iterator end()   const { return dict.end(); }
        typename std::unordered_map<TKey, TValue, Hasher>::iterator       end()         { return dict.end(); }

	private:

		std::unordered_map<TKey, TValue, Hasher> dict;
	};
}