#include "WorldGrid.h"

#include <algorithm>
#include "App/GameApp.h"
#include "App/AssetManager.h"
#include "App/PerfTimer.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/RenderManager.h"
#include "Gameplay/WorldGrid/TilePatch.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "Util/MapUtil.h"
#include "Util/Math.h"

//-----------------------------------------------------------------------------

#pragma region WorldGridCell

WorldGridCell::WorldGridCell(IntVec coords) :
	m_Coords(coords),
	m_Position(WorldGrid::CoordsToPosition(coords))
{ }

IntRect WorldGridCell::GetBox() const
{
	return IntRect(m_Coords.x * c_PatchWidth, m_Coords.y * c_PatchHeight, c_PatchWidth, c_PatchHeight);
}

void WorldGridCell::AddTile(IntVec position, int32 tileId)
{
	IntVec tileXY = PositionToTileXY(position);

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

HitResult WorldGridCell::CheckForSolidTile(IntRect rect) const
{
	if (!m_TilePatch)
	{
		return HitResult();
	}

	// Find the part of the rectangle that's inside this patch.
	const IntVec startTile = ClampedPositionToTileXY(IntVec(rect.left, rect.top));
	const IntVec endTile = ClampedPositionToTileXY(IntVec(rect.left + rect.width - 1, rect.top + rect.height - 1));

	for (int y = startTile.y; y <= endTile.y; ++y)
	{
		for (int x = startTile.x; x <= endTile.x; ++x)
		{
			const IntVec tileXY(x,y);
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

HitResult WorldGridCell::CheckForSolidObject(IntRect rect, GameObject* ignore) const
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

void WorldGridCell::GatherDraw(RenderManager& renderManager) const
{
	if (m_TilePatch)
	{
		sf::Transform transform;
		transform.translate(ToFVec(m_Position));
		renderManager.AddDrawable(RenderLayer::MainTiles_Lit, m_TilePatch.get(), transform);
	}

	m_ObjectBucket.GatherDraw(renderManager);
}

IntVec WorldGridCell::PositionToTileXY(IntVec tilePosition) const
{
	assert(GetBox().contains(tilePosition));
	
	IntVec relativePosition = tilePosition - m_Position;
	
	assert(relativePosition.x >= 0 && relativePosition.x < c_PatchWidth);
	assert(relativePosition.y >= 0 && relativePosition.y < c_PatchHeight);

	return IntVec(relativePosition.x / c_TileWidth, relativePosition.y / c_TileHeight);
}

IntVec WorldGridCell::ClampedPositionToTileXY(IntVec tilePosition) const
{
	IntVec relativePosition = tilePosition - m_Position;

	int32 tileX = Math::Clamp(relativePosition.x / c_TileWidth, 0, c_PatchTiles - 1);
	int32 tileY = Math::Clamp(relativePosition.y / c_TileHeight, 0, c_PatchTiles - 1);

	return IntVec(tileX, tileY);
}

IntVec WorldGridCell::TileXYToPosition(IntVec tileXY) const
{
	return m_Position + IntVec(c_TileWidth * tileXY.x, c_TileHeight * tileXY.y);
}

#pragma endregion WorldGridCell

//-----------------------------------------------------------------------------

#pragma region WorldCell

/*static*/ IntVec WorldGrid::PositionToCoords(IntVec worldPosition)
{
	IntVec cell;

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

/*static*/ IntVec WorldGrid::CoordsToPosition(IntVec coords)
{
	return IntVec(coords.x * c_PatchWidth, coords.y * c_PatchHeight);
}

WorldGridCell& WorldGrid::GetCell(IntVec cell)
{
	auto pair = m_CellMap.try_emplace(cell, WorldGridCell(cell));
	return pair.first->second;
}

WorldGridCell& WorldGrid::GetCellForPosition(IntVec worldPosition)
{
	return GetCell(PositionToCoords(worldPosition));
}

void WorldGrid::AddTile(IntVec position, int32 tileId)
{
	WorldGridCell& Cell = GetCellForPosition(position);
	Cell.AddTile(position, tileId);
}

void WorldGrid::BuildVertexArrays()
{
	// Todo: Probably something more reasonable about tilesets and textures.
	AssetManager& AssetManager = GameApp::GetAssetManager();
	m_Tileset.m_Texture = AssetManager.FindTexture("Tileset");
	m_Tileset.m_TilesPerRow = 20; // TODO this should really come from the tiled map

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
			IntVec coords = PositionToCoords(object->GetTopLeft());
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
		IntVec oldCoords = object->GetCachedGridCoords();
		IntVec newCoords = PositionToCoords(object->GetTopLeft());
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
	PerfTimer timer(__FUNCTION__);
	assert(m_TickList.empty());

	ForEachCellInRect(tickArea,
		[this](WorldGridCell& cell)
		{
			cell.GetObjectBucket().GatherAllObjects(m_TickList);
			return true; // continue processing
		}
	);

	m_PersistentBucket.GatherAllObjects(m_TickList);

	for (ObjectRef& ref : m_TickList)
	{
		if (GameObject* object = ref.Get())
		{
			object->GameObjectTick(deltaTime);
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

void WorldGrid::GatherDraw(RenderManager& renderManager, sf::IntRect gatherRect) const
{
	PerfTimer timer(__FUNCTION__);

	{
		// TODO: This should happen in light components
		//ShaderManager& shaderManager = GameApp::GetShaderManager();
		//shaderManager.AddPointLight( {100.0f, 100.0f}, {1.0f, 0.0f, 0.0f}, 300.0f);
		//shaderManager.AddPointLight( {300.0f, 150.0f}, {1.0f, 0.8f, 0.0f}, 300.0f);
		//shaderManager.AddPointLight( {700.0f, 300.0f}, {0.0f, 0.0f, 1.0f}, 400.0f);
	}

	ForEachCellInRect(gatherRect, [&renderManager](const WorldGridCell& cell)
	{
		cell.GatherDraw(renderManager);
		return true; // continue iterating
	});

	m_PersistentBucket.GatherDraw(renderManager);
}

void WorldGrid::ForEachCellInRect(IntRect rect, std::function<bool(WorldGridCell&)> lambda)
{
	const IntVec startCell = PositionToCoords({rect.left, rect.top});
	const IntVec endCell = PositionToCoords({rect.left+rect.width, rect.top + rect.height});
	for (int32 x = startCell.x; x <= endCell.x; ++x)
	{
		for (int32 y = startCell.y; y <= endCell.y; ++y)
		{
			if (WorldGridCell* cell = Util::Find(m_CellMap, {x,y}))
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

void WorldGrid::ForEachCellInRect(IntRect rect, std::function<bool(const WorldGridCell&)> lambda) const
{
	const IntVec startCell = PositionToCoords({rect.left, rect.top});
	const IntVec endCell = PositionToCoords({rect.left+rect.width, rect.top + rect.height});
	for (int32 x = startCell.x; x <= endCell.x; ++x)
	{
		for (int32 y = startCell.y; y <= endCell.y; ++y)
		{
			if (const WorldGridCell* cell = Util::Find(m_CellMap, {x,y}))
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
