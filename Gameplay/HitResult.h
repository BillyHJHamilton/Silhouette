#pragma once

#include "Gameplay.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Util/Vec2.h"

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
	IntVec m_TilePosition = IntVec{0,0};

	// Filled if object is hit.
	ObjectRef m_Object;

	HitResult() :
		m_Result(NoHit)
	{ }

	HitResult(int32 tileId, IntVec tilePosition) :
		m_Result(HitTile),
		m_TileId(tileId),
		m_TilePosition(tilePosition)
	{ }

	HitResult(ObjectRef object) :
		m_Result(object ? HitObject : NoHit),
		m_Object(object)
	{ }

	bool IsHit() const
	{
		return m_Result != Result::NoHit;
	}
};
