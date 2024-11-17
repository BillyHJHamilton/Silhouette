#include "WorldGrid.h"

#include <algorithm>
#include "App/GameApp.h"
#include "App/AssetManager.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/WorldGrid/TilePatch.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "Util/MapUtil.h"
#include "Util/Math.h"

//-----------------------------------------------------------------------------

#pragma region WorldGridCell

WorldGridCell::WorldGridCell(Vector2i coords) :
	m_Coords(coords),
	m_Position(WorldGrid::CoordsToPosition(coords))
{ }

IntRect WorldGridCell::GetBox() const
{
	return IntRect(m_Coords.x * c_PatchWidth, m_Coords.y * c_PatchHeight, c_PatchWidth, c_PatchHeight);
}

void WorldGridCell::AddTile(Vector2i position, int32 tileId)
{
	Vector2i tileXY = PositionToTileXY(position);

	if (!m_TilePatch)
	{
		m_TilePatch = std::make_shared<TilePatch>();
	}

	m_TilePatch->SetTileId(tileXY, tileId);
}

void WorldGridCell::BuildVertexArray(Tileset* tileset)
{
	if (m_TilePatch)
	{
		m_TilePatch->SetTileset(tileset);
		m_TilePatch->CreateVertexArray(tileset->m_TilesPerRow);
	}
}

HitResult WorldGridCell::CheckForSolidTile(sf::IntRect rect) const
{
	// Find the part of the rectangle that's inside this patch.
	const Vector2i startTile = ClampedPositionToTileXY(Vector2i(rect.left, rect.top));
	const Vector2i endTile = ClampedPositionToTileXY(Vector2i(rect.left + rect.width, rect.top + rect.height));

	for (int y = startTile.y; y <= endTile.y; ++y)
	{
		for (int x = startTile.x; x <= endTile.x; ++x)
		{
			const Vector2i tileXY(x,y);
			const int32 tileId = m_TilePatch->GetTileId(tileXY);

			// For now, consider all tiles solid.  We can add a parameter later.
			if (tileId != c_InvalidTile)
			{
				return HitResult(tileId, TileXYToPosition(tileXY));
			}
		}
	}

	return HitResult(); // NoHit
}

HitResult WorldGridCell::CheckForSolidObject(sf::IntRect rect, GameObject* ignore) const
{
	const ObjectRef ref = m_ObjectBucket.FindFirstHitByChannel(rect, "Solid", ignore);
	if (ref)
	{
		return HitResult(ref);
	}
	else
	{
		return HitResult(); // NoHit
	}
}

void WorldGridCell::draw(RenderTarget& target, RenderStates states) const
{
	if (m_TilePatch)
	{
		states.transform.translate(FVec(m_Position));
		target.draw(*m_TilePatch, states);
	}
}

Vector2i WorldGridCell::PositionToTileXY(Vector2i tilePosition) const
{
	assert(GetBox().contains(tilePosition));
	
	Vector2i relativePosition = tilePosition - m_Position;
	
	assert(relativePosition.x >= 0 && relativePosition.x < c_PatchWidth);
	assert(relativePosition.y >= 0 && relativePosition.y < c_PatchHeight);

	return Vector2i(relativePosition.x / c_TileWidth, relativePosition.y / c_TileHeight);
}

Vector2i WorldGridCell::ClampedPositionToTileXY(Vector2i tilePosition) const
{
	Vector2i relativePosition = tilePosition - m_Position;

	int32 tileX = Math::Clamp(relativePosition.x / c_TileWidth, 0, c_PatchTiles - 1);
	int32 tileY = Math::Clamp(relativePosition.y / c_TileHeight, 0, c_PatchTiles - 1);

	return Vector2i(tileX, tileY);
}

Vector2i WorldGridCell::TileXYToPosition(Vector2i tileXY) const
{
	return m_Position + Vector2i(c_TileWidth * tileXY.x, c_TileHeight * tileXY.y);
}

#pragma endregion WorldGridCell

//-----------------------------------------------------------------------------

#pragma region WorldCell

/*static*/ Vector2i WorldGrid::PositionToCoords(Vector2i worldPosition)
{
	Vector2i cell;

	if (worldPosition.x >= 0)
	{
		cell.x = worldPosition.x / c_PatchWidth;
	}
	else
	{
		// Round away from 0
		cell.x = (worldPosition.x - (c_PatchWidth - 1)) / c_PatchWidth;
	}

	if (worldPosition.y >= 0)
	{
		cell.y = worldPosition.y / c_PatchHeight;
	}
	else
	{
		// Round away from 0
		cell.y = (worldPosition.y - (c_PatchHeight - 1)) / c_PatchHeight;
	}

	return cell;
}

/*static*/ Vector2i WorldGrid::CoordsToPosition(Vector2i coords)
{
	return Vector2i(coords.x * c_PatchWidth, coords.y * c_PatchHeight);
}

WorldGridCell& WorldGrid::GetCell(Vector2i cell)
{
	auto pair = m_CellMap.try_emplace(cell, WorldGridCell(cell));
	return pair.first->second;
}

WorldGridCell& WorldGrid::GetCellForPosition(Vector2i worldPosition)
{
	return GetCell(PositionToCoords(worldPosition));
}

void WorldGrid::AddTile(Vector2i position, int32 tileId)
{
	WorldGridCell& Cell = GetCellForPosition(position);
	Cell.AddTile(position, tileId);
}

void WorldGrid::BuildVertexArrays()
{
	// Todo: Probably something more reasonable about tilesets and textures.
	AssetManager& AssetManager = GameApp::GetAssetManager();
	m_Tileset.m_Texture = AssetManager.FindTexture("Tileset");
	m_Tileset.m_TilesPerRow = 2;

	for (auto& pair : m_CellMap)
	{
		pair.second.BuildVertexArray(&m_Tileset);
	}
}

void WorldGrid::AddObject(GameObject* object)
{
	assert(object);
	if (object)
	{
		if (object->IsPersistent())
		{
			m_PersistentBucket.AddObject(object);
		}
		else
		{
			Vector2i coords = PositionToCoords(object->GetTopLeft());
			WorldGridCell& cell = GetCell(coords);
			object->SetCachedGridCoords(coords);
			cell.GetObjectBucket().AddObject(object);
		}
	}
}

void WorldGrid::UpdateObjectPosition(GameObject* object)
{
	assert(object);
	if (object && !object->IsPersistent())
	{
		Vector2i oldCoords = object->GetCachedGridCoords();
		Vector2i newCoords = PositionToCoords(object->GetTopLeft());
		if (oldCoords != newCoords)
		{
			WorldGridCell& oldCell = GetCell(oldCoords);
			WorldGridCell& newCell = GetCell(newCoords);
			oldCell.GetObjectBucket().TransferObject(object, newCell.GetObjectBucket());
		}
	}
}

void WorldGrid::TickObjects(float deltaTime, IntRect tickArea)
{
	assert(m_TickList.empty());

	ForEachCellInRect(tickArea,
		[this](WorldGridCell& cell)
		{
			cell.GetObjectBucket().GatherAllObjects(m_TickList);
			return true; // continue processing
		}
	);

	for (ObjectRef& ref : m_TickList)
	{
		if (GameObject* object = ref.Get())
		{
			object->Tick(deltaTime);
		}
	}

	m_TickList.clear();
}

HitResult WorldGrid::CheckForSolid(IntRect rect, GameObject* ignore) const
{
	HitResult result;

	// First check for a solid tile.
	ForEachCellInRect(rect,
		[&result, rect](const WorldGridCell& cell)
		{
			result = cell.CheckForSolidTile(rect);
			return !result.IsHit();
		}
	);

	if (!result.IsHit())
	{
		// No tile, so check for a solid object.
		// We add some padding so if rect is on the edge of a cell, we also check the adjacent one.
		const IntRect paddedRect(rect.left - c_PatchWidth/2, rect.top - c_PatchHeight/2,
			rect.width + c_PatchWidth, rect.height + c_PatchHeight);
		ForEachCellInRect(paddedRect,
			[&result, ignore, rect](const WorldGridCell& cell)
			{
				result = cell.CheckForSolidObject(rect, ignore);
				return !result.IsHit();
			}
		);
	}

	return result;
}

void WorldGrid::draw(RenderTarget& target, RenderStates states) const
{
	for (auto& pair : m_CellMap)
	{
		target.draw(pair.second, states);
	}
}

void WorldGrid::ForEachCellInRect(sf::IntRect rect, std::function<bool(WorldGridCell&)> lambda)
{
	for (int32 x = rect.left; x < rect.left + rect.width; x += c_PatchWidth)
	{
		for (int32 y = rect.top; y < rect.top + rect.height; x += c_PatchHeight)
		{
			const Vector2i coords = PositionToCoords(Vector2i(x,y));
			if (WorldGridCell* cell = Util::MapFind(m_CellMap, coords))
			{
				const bool keepLooping = lambda(*cell);
				if (!keepLooping)
				{
					return;
				}
			}
		}
	}
}

void WorldGrid::ForEachCellInRect(sf::IntRect rect, std::function<bool(const WorldGridCell&)> lambda) const
{
	for (int32 x = rect.left; x < rect.left + rect.width; x += c_PatchWidth)
	{
		for (int32 y = rect.top; y < rect.top + rect.height; x += c_PatchHeight)
		{
			const Vector2i coords = PositionToCoords(Vector2i(x,y));
			if (const WorldGridCell* cell = Util::MapFind(m_CellMap, coords))
			{
				const bool keepLooping = lambda(*cell);
				if (!keepLooping)
				{
					return;
				}
			}
		}
	}
}

#pragma endregion WorldCell
