#pragma once

#include "Platform.h"

#include <unordered_map>


namespace WFC
{
	template<typename TKey, typename TValue>
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
	    TValue& At(const TKey& key) { return dict.at(key); } //Needed for the debugger, it can't recognize subscript operators

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
			return (TValue*)((const Dictionary<TKey, TValue>*)this)->TryGet(key);
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
			return (TValue&)((const Dictionary<TKey, TValue>*)this)->Get(key, valIfNotFound);
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

		const auto& Unwrap() const { return dict; }
		auto& Unwrap() { return dict; }

        //Iterators for ranged-for loops.
        //We can't use "auto" for the return type here because it breaks SWIG.
        typename std::unordered_map<TKey, TValue>::const_iterator begin() const { return dict.begin(); }
        typename std::unordered_map<TKey, TValue>::iterator       begin()       { return dict.begin(); }
        typename std::unordered_map<TKey, TValue>::const_iterator end()   const { return dict.end(); }
        typename std::unordered_map<TKey, TValue>::iterator       end()         { return dict.end(); }

	private:

		std::unordered_map<TKey, TValue> dict;
	};
}