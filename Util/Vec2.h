#pragma once

#include "Core/BasicTypes.h"
#include "SFML/System/Vector2.hpp"

using namespace sf;

// Convert between vectors without verbose typecasting.

inline Vector2f FVec(int32 x, int32 y)
{
	return Vector2f(static_cast<float>(x), static_cast<float>(y));
}

inline Vector2f FVec(Vector2i v)
{
	return Vector2f(static_cast<float>(v.x), static_cast<float>(v.y));
}

// Support for unordered_map<Vector2i>
namespace std
{
	template <>
	struct hash<Vector2i>
	{
		size_t operator()(const Vector2i& v) const
		{
			return static_cast<size_t>(v.x) & (static_cast<size_t>(v.y) << 32);
		}
	};
}
