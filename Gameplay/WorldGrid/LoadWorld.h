#pragma once

#include <iostream>
#include <unordered_map>
#include "Core/Core.h"
#include "Core/NameHash.h"
#include "Gameplay/Gameplay.h"
#include "Tileset.h"

namespace pugi
{
	class xml_node;
}

void LoadTilesFromCsv(std::string filename, WorldGrid& worldGrid);

class TmxParser
{
public:
	// tmxFilename - Path to the tmx file containing the map.
	// tilesetPath - Path to folder containing tsx files for tilesets, with trailling slash.
	// world - World where tiles and objects will be added when loaded.
	void LoadMapFromTmx(std::string tmxFilename, std::string tilesetPath, World& world);

protected:
	struct TilesetInfo
	{
		int32 m_FirstGid;
		int32 m_LastGid;
		std::unordered_map<int32, NameHash> m_GidToName;
	};

	void ParseTilesetInfo(pugi::xml_node mapNode, std::string tilesetPath);
	void LoadTilesFromStream(std::istream& stream, WorldGrid& worldGrid);

	TilesetInfo* InfoForGid(int32 gid);
	int32 GidToTileId(int32 gid);
	NameHash GidToObjectName(int32 gid);

	std::vector<TilesetInfo> m_TilesetInfoList;
};
