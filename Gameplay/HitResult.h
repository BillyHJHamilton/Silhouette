#pragma once

#include "Gameplay.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "SFML/System/Vector2.hpp"

// Returned when we check if there's something solid at a location.
struct HitResult
{
	enum Result
	{
		NoHit,
		HitTile,
		HitObject
	};
	Result m_Result = NoHit;

	// Filled if tile is hit.
	int32 m_TileId = c_InvalidTile;
	sf::Vector2i m_TilePosition = sf::Vector2i{0,0};

	// Filled if object is hit.
	ObjectRef m_Object;

	HitResult() :
		m_Result(NoHit)
	{ }

	HitResult(int32 tileId, sf::Vector2i tilePosition) :
		m_Result(HitTile),
		m_TileId(tileId),
		m_TilePosition(tilePosition)
	{ }

	HitResult(ObjectRef object) :
		m_Result(HitObject),
		m_Object(object)
	{ }

	bool IsHit() const
	{
		return m_Result != Result::NoHit;
	}
};
