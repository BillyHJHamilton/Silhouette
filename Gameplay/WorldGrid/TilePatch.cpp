#include "TilePatch.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "Util/Vec2.h"

TilePatch::TilePatch()
{
	for (int y = 0; y < c_PatchTiles; ++y)
	{
		for (int x = 0; x < c_PatchTiles; ++x)
		{
			m_TileIdGrid[y][x] = c_InvalidTile;
		}
	}
}

void TilePatch::SetTileset(Tileset* tileset)
{
	m_Tileset = tileset;
}

void TilePatch::SetTileId(Vector2i tileXY, int32 tileId)
{
	m_TileIdGrid[tileXY.y][tileXY.x] = tileId;
}

int32 TilePatch::GetTileId(Vector2i tileXY) const
{
	return m_TileIdGrid[tileXY.y][tileXY.x];
}

void TilePatch::CreateVertexArray(int32 tilesPerRow)
{
	//assert(tileset != nullptr);
	//assert(data != nullptr);

	const Vector2i tileSize(c_TileWidth, c_TileHeight);
	int32 numValidTiles = CountValidTiles();

	m_VertexArray.setPrimitiveType(Quads);
	m_VertexArray.resize(numValidTiles*4LL);

	// Populate vertex array with one quad per tile.
	int32 quadIndex = 0;
	for (int y = 0; y < c_PatchTiles; ++y)
	{
		for (int x = 0; x < c_PatchTiles; ++x)
		{
			const int32 tileId = m_TileIdGrid[y][x];
			if (tileId != c_InvalidTile)
			{
				// TODO: Find position in tileset texture, once multiple tiles exist.
				int32 tileX = tileId % tilesPerRow;
				int32 tileY = tileId / tilesPerRow;

				sf::Vertex& v0 = m_VertexArray[quadIndex*4LL];
				sf::Vertex& v1 = m_VertexArray[quadIndex*4LL + 1];
				sf::Vertex& v2 = m_VertexArray[quadIndex*4LL + 2];
				sf::Vertex& v3 = m_VertexArray[quadIndex*4LL + 3];

				v0.position = FVec(x * tileSize.x, y * tileSize.y);
				v1.position = FVec((x+1) * tileSize.x, y * tileSize.y);
				v2.position = FVec((x+1) * tileSize.x, (y+1) * tileSize.y);
				v3.position = FVec(x * tileSize.x, (y+1) * tileSize.y);

				v0.texCoords = FVec(tileX * tileSize.x, tileY * tileSize.y);
				v1.texCoords = FVec((tileX+1) * tileSize.x, tileY * tileSize.y);
				v2.texCoords = FVec((tileX+1) * tileSize.x, (tileY+1) * tileSize.y);
				v3.texCoords = FVec(tileX * tileSize.x, (tileY+1) * tileSize.y);

				++quadIndex;
			}
		}
	}
}

void TilePatch::draw(RenderTarget& target, RenderStates states) const
{
	if (m_Tileset)
	{
		states.transform *= getTransform();
		states.texture = m_Tileset->m_Texture;
		target.draw(m_VertexArray, states);
	}
}

int32 TilePatch::CountValidTiles()
{
	int32 count = 0;

	for (int y = 0; y < c_PatchTiles; ++y)
	{
		for (int x = 0; x < c_PatchTiles; ++x)
		{
			if (m_TileIdGrid[y][x] != c_InvalidTile)
			{
				++count;
			}
		}
	}

	return count;
}
