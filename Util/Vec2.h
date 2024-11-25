#pragma once

#include <cmath>
#include "Core/BasicTypes.h"
#include "SFML/System/Vector2.hpp"

using IntVec = sf::Vector2i;
using FVec = sf::Vector2f;

// Convert between vectors without verbose typecasting.
inline FVec ToFVec(int32 x, int32 y)
{
	return FVec(static_cast<float>(x), static_cast<float>(y));
}
inline FVec ToFVec(IntVec v)
{
	return FVec(static_cast<float>(v.x), static_cast<float>(v.y));
}
inline IntVec RoundToIntVec(FVec v)
{
	return IntVec(static_cast<int32>(lround(v.x)), static_cast<int32>(lround(v.y)));
}

// Support for unordered_map<IntVec>
namespace std
{
	template <>
	struct hash<IntVec>
	{
		size_t operator()(const IntVec& v) const
		{
			return static_cast<size_t>(v.x) & (static_cast<size_t>(v.y) << 32);
		}
	};
}
