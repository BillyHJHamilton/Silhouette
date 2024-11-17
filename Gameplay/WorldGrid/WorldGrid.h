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
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "Util/Vec2.h"

using namespace sf;

class TilePatch;
struct Tileset;

// Spatial partition.

class WorldGridCell : public Drawable
{
public:
	WorldGridCell(Vector2i coords);
	IntRect GetBox() const;

	void AddTile(Vector2i position, int32 tileId);
	void BuildVertexArray(Tileset* tileset);

	const ObjectBucket& GetObjectBucket() const { return m_ObjectBucket; }
	ObjectBucket& GetObjectBucket() { return m_ObjectBucket; }

	HitResult CheckForSolidTile(sf::IntRect rect) const;
	HitResult CheckForSolidObject(sf::IntRect rect, GameObject* ignore = nullptr) const;

protected:
	// Drawable
	virtual void draw(RenderTarget& target, RenderStates states) const override;

	Vector2i PositionToTileXY(Vector2i position) const;
	Vector2i ClampedPositionToTileXY(Vector2i position) const;

	Vector2i TileXYToPosition(Vector2i tileXY) const;

	Vector2i m_Coords;
	Vector2i m_Position;

	// If needed, we could have a map or array of patches per layer in the future.
	std::shared_ptr<TilePatch> m_TilePatch;

	ObjectBucket m_ObjectBucket;
};

class WorldGrid : public Drawable
{
public:
	static Vector2i PositionToCoords(Vector2i worldPosition);
	static Vector2i CoordsToPosition(Vector2i cellCoordinates);

	// Gets the cell, creating it if it does not exist.
	WorldGridCell& GetCell(Vector2i cell);
	WorldGridCell& GetCellForPosition(Vector2i worldPosition);

	void AddTile(Vector2i position, int32 tileId);
	void BuildVertexArrays();

	void AddObject(GameObject* object);
	void UpdateObjectPosition(GameObject* object);
	void TickObjects(float deltaTime, sf::IntRect tickArea);

	HitResult CheckForSolid(sf::IntRect rect, GameObject* ignore = nullptr) const;

protected:
	// Drawable
	virtual void draw(RenderTarget& target, RenderStates states) const override;

	// The bool return value is "Keep looping?", so return false if you're all done.
	void ForEachCellInRect(sf::IntRect rect, std::function<bool(WorldGridCell&)> lambda);
	void ForEachCellInRect(sf::IntRect rect, std::function<bool(const WorldGridCell&)> lambda) const;

	std::unordered_map<Vector2i,WorldGridCell> m_CellMap;
	ObjectBucket m_PersistentBucket;

	// Only used in one function, but member variable to reduce allocations.
	ObjectRefList m_TickList;

	// Todo: Probably something more reasonable than this
	Tileset m_Tileset;
};
