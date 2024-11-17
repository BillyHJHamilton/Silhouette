#pragma once

#include "Core/BasicTypes.h"
#include "SFML/System/Vector2.hpp"

static constexpr int32 c_InvalidTile = -1;

static constexpr int32 c_TileWidth = 16;
static constexpr int32 c_TileHeight = 16;

static constexpr int32 c_PatchTiles = 20; // In each dimension

static constexpr int32 c_PatchWidth = c_TileWidth*c_PatchTiles;
static constexpr int32 c_PatchHeight = c_TileHeight*c_PatchTiles;
