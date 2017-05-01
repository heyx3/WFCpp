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

		//A function that just takes a const reference to a key.
		template<typename Func>
		//Runs the given function for every key in this dictionary.
		void DoToEach(Func func) const
		{
			for (auto& keyAndVal : dict)
				func(keyAndVal.first);
		}

		//A function that just takes const references to a key and its value.
		template<typename Func>
		//Runs the given function for every key/value pair in this dictionary.
		void DoToEach_Pair(Func func) const
		{
			for (auto& keyAndVal : dict)
				func(keyAndVal.first, keyAndVal.second);
		}


	private:

		std::unordered_map<TKey, TValue> dict;
	};

	#define EXPORT_WFC_DICT(keyType, valType) template class WFC_API Dictionary<keyType, valType>;
}