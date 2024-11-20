#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace Util
{
	// Basic functions for searching.

	template<typename VectorItemType, typename ValueType>
	auto Find(std::vector<VectorItemType> vector, ValueType value)
	{
		return std::find(vector.begin(), vector.end(), value);
	}

	template<typename VectorItemType, typename ValueType>
	bool Contains(std::vector<VectorItemType>& vector, ValueType& value)
	{
		auto itr = std::find(vector.begin(), vector.end(), value);
		return itr != vector.end();
	}

	// Functions for searching an array of unique_ptrs when you have a normal pointer.

	template<typename PointerType>
	auto Find(std::vector<std::unique_ptr<PointerType>>& vector, PointerType* pointer)
	{
		return std::find_if(vector.begin(), vector.end(),
			[pointer](std::unique_ptr<PointerType>& entry)
			{
				return entry.get() == pointer;
			}
		);
	}

	template<typename PointerType>
	bool Contains(std::vector<std::unique_ptr<PointerType>>& vector, PointerType* pointer)
	{
		auto itr = Find(vector, pointer);
		return itr != vector.end();
	}

	// Utility functions for removing items from std::vector without maintaining order.

	template<typename VectorItemType>
	void RemoveSwap(std::vector<VectorItemType>& vector, int indexToRemove)
	{
		assert(indexToRemove < vector.size());
		vector[indexToRemove] = std::move(vector.back());
		vector.pop_back();
	}

	template<typename VectorItemType>
	void RemoveSwap(std::vector<VectorItemType>& vector, typename std::vector<VectorItemType>::iterator& itrToRemove)
	{
		assert(itrToRemove != vector.cend());
		auto backItr = vector.end() - 1;
		*itrToRemove = std::move(*backItr);
		vector.pop_back();
	}

	template<typename VectorItemType, typename ValueType>
	void RemoveSwapFirstMatchingItem(std::vector<VectorItemType>& vector, ValueType& value)
	{
		auto itr = Find(vector, value);
		if (itr != vector.end())
		{
			RemoveSwap(vector, itr);
		}
	}

	template<typename VectorItemType, typename ValueType>
	void RemoveSwapAllMatchingItems(std::vector<VectorItemType>& vector, ValueType& value)
	{
		for (int i = 0; i < vector.size(); ++i)
		{
			if (vector[i] == value)
			{
				RemoveSwap(vector, i);
				--i;
			}
		}
	}

	template<typename VectorItemType>
	void RemoveSwapAllNullItems(std::vector<VectorItemType>& vector)
	{
		for (int i = 0; i < vector.size(); ++i)
		{
			if (vector[i] == nullptr)
			{
				RemoveSwap(vector, i);
				--i;
			}
		}
	}
	
	// Usable only if an IsValid function has been defined for the item type.
	//template<typename VectorItemType>
	//void RemoveSwapAllInvalidItems(std::vector<VectorItemType>& vector)
	//{
	//	for (int i = 0; i < vector.size(); ++i)
	//	{
	//		const VectorItemType& item = vector[i];
	//		if (!IsValid(item))
	//		{
	//			RemoveSwap(vector, i);
	//			--i;
	//		}
	//	}
	//}

	// Adds the second vector to the end of the first vector.  Second vector is unchanged.
	template<typename VectorItemType>
	void Append(std::vector<VectorItemType>& first, const std::vector<VectorItemType>& second)
	{
		first.insert(first.end(), second.begin(), second.end());
	}

	// Sorts the list with std::sort.
	// Ascending requires operator< while descending requires operator>.
	template<typename VectorItemType>
	void SortAscending(std::vector<VectorItemType>& v)
	{
		std::sort(v.begin(), v.end());
	}
	template<typename VectorItemType>
	void SortDescending(std::vector<VectorItemType>& v)
	{
		std::sort(v.begin(), v.end(), std::greater<VectorItemType>());
	}
}
