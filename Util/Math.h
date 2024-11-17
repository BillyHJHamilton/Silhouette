#pragma once

#include <cmath>
#include "Core/BasicTypes.h"
#include "SFML/System/Vector2.hpp"

using namespace sf;

namespace Math
{
	inline int32 Sign(int32 x)
	{
		return (x > 0) ? 1 : (x == 0) ? 0 : -1;
	}

	template<typename T>
	inline T Clamp(T value, T low, T high)
	{
		return (value < low) ? low : (value < high) ? value : high;
	}

	inline float LengthSquared(Vector2f v)
	{
		return v.x*v.x + v.y*v.y;
	}
	inline float Length(Vector2f v)
	{
		return std::sqrt(LengthSquared(v));
	}
	inline float DistanceSquared(Vector2f p1, Vector2f p2)
	{
		return LengthSquared(p2 - p1);
	}
	inline float Distance(Vector2f p1, Vector2f p2)
	{
		return Length(p2 - p1);
	}
	inline float DegToRad(float degrees)
	{
		return 0.01745329252f * degrees;
	}
	inline float SinDeg(float degrees)
	{
		return std::sinf(DegToRad(degrees));
	}
	inline float CosDeg(float degrees)
	{
		return std::cosf(DegToRad(degrees));
	}
	inline Vector2f RotationToUnitVector(float sfRotation)
	{
		return
		{
			Math::SinDeg(sfRotation),
			-Math::CosDeg(sfRotation)
		};
	}
};
