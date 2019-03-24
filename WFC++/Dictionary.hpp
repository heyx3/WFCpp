#pragma once

#include "Platform.h"

#include <unordered_map>


namespace WFC
{
	template<typename TKey, typename TValue>
	//A wrapper around std::unordered_map so that it can cross DLL boundaries.
	class WFC_API Dictionary
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

        //Iterators for ranged-for loops.
        auto begin() const { return dict.begin(); }
        auto begin() { return dict.begin(); }
        auto end() const { return dict.end(); }
        auto end() { return dict.end(); }

	private:

		std::unordered_map<TKey, TValue> dict;
	};

	#define EXPORT_WFC_DICT(keyType, valType) template class WFC_API Dictionary<keyType, valType>;
}