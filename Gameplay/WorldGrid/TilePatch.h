#pragma once

#include <array>
#include "Core/Core.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Gameplay/WorldGrid/Tileset.h"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/Graphics/VertexArray.hpp"

using namespace sf;

// Loosely based on tutorial from
// https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
// Modified to allow a sparse grid.

class TilePatch : public Drawable, public Transformable
{
public:
	TilePatch();

	void SetTileset(Tileset* tileset);
	void SetTileId(Vector2i tileXY, int32 tileId);

	int32 GetTileId(Vector2i tileXY) const;

	void CreateVertexArray(int32 tilesPerRow);

protected:
	virtual void draw(RenderTarget& target, RenderStates states) const;

	int32 CountValidTiles();

	// 2D array of tile id at each position in the patch.  Index as [y][x]
	std::array<std::array<int32,c_PatchTiles>,c_PatchTiles> m_TileIdGrid;

	Tileset* m_Tileset = nullptr;
	VertexArray m_VertexArray;
};
