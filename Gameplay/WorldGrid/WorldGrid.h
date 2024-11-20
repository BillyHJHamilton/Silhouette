#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Core/Core.h"
#include "Gameplay/HitResult.h"
#include "Gameplay/WorldGrid/ObjectBucket.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Gameplay/WorldGrid/Tileset.h"
#include "SFML/Graphics/Drawable.hpp"
#include "Util/Rect.h"
#include "Util/Vec2.h"

class TilePatch;
struct Tileset;

// Spatial partition.

class WorldGridCell : public sf::Drawable
{
public:
	WorldGridCell(IntVec coords);
	IntRect GetBox() const;

	void AddTile(IntVec position, int32 tileId);
	void BuildVertexArray(Tileset* tileset);

	const ObjectBucket& GetObjectBucket() const { return m_ObjectBucket; }
	ObjectBucket& GetObjectBucket() { return m_ObjectBucket; }

	HitResult CheckForSolidTile(IntRect rect) const;
	HitResult CheckForSolidObject(IntRect rect, GameObject* ignore = nullptr) const;

protected:
	// Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	IntVec PositionToTileXY(IntVec position) const;
	IntVec ClampedPositionToTileXY(IntVec position) const;

	IntVec TileXYToPosition(IntVec tileXY) const;

	IntVec m_Coords;
	IntVec m_Position;

	// If needed, we could have a map or array of patches per layer in the future.
	std::shared_ptr<TilePatch> m_TilePatch;

	ObjectBucket m_ObjectBucket;
};

class WorldGrid : public sf::Drawable
{
public:
	static IntVec PositionToCoords(IntVec worldPosition);
	static IntVec CoordsToPosition(IntVec cellCoordinates);

	// Gets the cell, creating it if it does not exist.
	WorldGridCell& GetCell(IntVec cell);
	WorldGridCell& GetCellForPosition(IntVec worldPosition);

	void AddTile(IntVec position, int32 tileId);
	void BuildVertexArrays();

	void AddObject(GameObject* object);
	void UpdateObjectPosition(GameObject* object);
	void TickObjects(float deltaTime, IntRect tickArea);

	HitResult CheckForSolid(IntRect rect, GameObject* ignore = nullptr) const;

protected:
	// Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// The bool return value is "Keep looping?", so return false if you're all done.
	void ForEachCellInRect(IntRect rect, std::function<bool(WorldGridCell&)> lambda);
	void ForEachCellInRect(IntRect rect, std::function<bool(const WorldGridCell&)> lambda) const;

	std::unordered_map<IntVec,WorldGridCell> m_CellMap;
	ObjectBucket m_PersistentBucket;

	// Only used in one function, but member variable to reduce allocations.
	ObjectRefList m_TickList;

	// Todo: Probably something more reasonable than this
	Tileset m_Tileset;
};
