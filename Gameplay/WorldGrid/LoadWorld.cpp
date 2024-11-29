#include "LoadWorld.h"

#include <fstream>
#include <sstream>
#include "External/Pugixml/pugixml.hpp"
#include "Gameplay/World.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Gameplay/WorldGrid/WorldGrid.h"
#include "Platformer/SpawnByName.h"
#include "Util/MapUtil.h"
#include "Util/Vec2.h"

void LoadTilesFromCsv(std::string filename, WorldGrid& worldGrid)
{
	std::ifstream inFile(filename);
	if (!inFile)
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}
	
	IntVec tilePos(0,0);

	int32 tilesRead = 0;
	while (inFile)
	{
		char next = inFile.peek();
		if (next == ',')
		{
			inFile.get(next);
			tilePos.x += c_TileWidth;
		}
		else if (next == '\n')
		{
			inFile.get(next);
			tilePos.x = 0;
			tilePos.y += c_TileHeight;
		}
		else
		{
			// A number, perhaps?
			int32 tileId = -1;
			inFile >> tileId;
			if (inFile && tileId != -1)
			{
				worldGrid.AddTile(tilePos, tileId);
				++tilesRead;
			}
		}
	}

	std::cout << "Read " << tilesRead << " tiles." << std::endl;
}

void TmxParser::LoadMapFromTmx(std::string tmxFilename, std::string tilesetPath, World& world)
{
	pugi::xml_document document;
	bool loaded = document.load_file(tmxFilename.c_str(), pugi::parse_default, pugi::xml_encoding::encoding_utf8);
	if (!loaded)
	{
		std::cerr << "Failed to parse Tiled map file: " << tmxFilename << std::endl;
		return;
	}

	pugi::xml_node mapNode = document.child("map");

	ParseTilesetInfo(mapNode, tilesetPath);

	// Process tile layer. (Todo: support multiple layers/tilesets.)
	pugi::xml_node tileLayer = mapNode.child("layer");

	// Get the tile data
	std::string tileData = tileLayer.child_value("data");
	std::stringstream tileDataStream(tileData);
	LoadTilesFromStream(tileDataStream, world.GetWorldGrid());

	// Process object layers.
	int32 objectsRead = 0;
	int32 areasRead = 0;
	for (pugi::xml_node objectGroupNode : mapNode.children("objectgroup"))
	{
		for (pugi::xml_node objectNode : objectGroupNode)
		{
			int32 gid = objectNode.attribute("gid").as_int();
			NameHash name = GidToObjectName(gid);
			if (name != "Invalid")
			{
				IntVec position(
					objectNode.attribute("x").as_int(),
					objectNode.attribute("y").as_int());
				SpawnByName(world, name, position);
				++objectsRead;
			}
			else
			{
				// Object has no gid, so it's probably an area.
				name = objectNode.attribute("type").as_string();
				IntRect rect(
					objectNode.attribute("x").as_int(),
					objectNode.attribute("y").as_int(),
					objectNode.attribute("width").as_int(),
					objectNode.attribute("height").as_int());
				if (rect.width > 0 && rect.height > 0)
				{
					pugi::xml_node propertiesNode = objectNode.child("properties");
					SpawnAreaByName(world, name, rect, propertiesNode);
					++areasRead;
				}
			}
		}
	}

	std::cout << "Read " << objectsRead << " objects." << std::endl;
	std::cout << "Read " << areasRead << " areas." << std::endl;
}

void TmxParser::LoadTilesFromStream(std::istream& stream, WorldGrid& worldGrid)
{
	IntVec tilePos(0,0);

	int32 tilesRead = 0;
	while (stream)
	{
		char next = stream.peek();
		if (next == ',')
		{
			stream.get(next);
			tilePos.x += c_TileWidth;
		}
		else if (next == '\n')
		{
			stream.get(next);
			if (tilePos.x > 0)
			{
				tilePos.x = 0;
				tilePos.y += c_TileHeight;
			}
		}
		else
		{
			// A number, perhaps?
			int32 gid = -1;
			stream >> gid;
			if (stream && gid > 0)
			{
				int32 tileId = GidToTileId(gid);
				worldGrid.AddTile(tilePos, tileId);
				++tilesRead;
			}
		}
	}

	if (tilesRead > 0)
	{
		std::cout << "Read " << tilesRead << " tiles." << std::endl;
	}
}

void TmxParser::ParseTilesetInfo(pugi::xml_node mapNode, std::string tilesetPath)
{
	for (pugi::xml_node mapTilesetNode : mapNode.children("tileset"))
	{
		TilesetInfo info;
		info.m_FirstGid = mapTilesetNode.attribute("firstgid").as_int();

		std::string tsxFilename = tilesetPath + mapTilesetNode.attribute("source").as_string();
		pugi::xml_document tilesetDocument;
		bool loaded = tilesetDocument.load_file(tsxFilename.c_str());
		if (loaded)
		{
			pugi::xml_node tilesetNode = tilesetDocument.child("tileset");

			int32 tilecount = tilesetNode.attribute("tilecount").as_int();
			info.m_LastGid = tilecount + info.m_FirstGid - 1;

			for (pugi::xml_node tileNode : tilesetNode.children("tile"))
			{
				int32 gid = tileNode.attribute("id").as_int() + info.m_FirstGid;
				NameHash hash = tileNode.attribute("type").as_string();
				info.m_GidToName.emplace(gid, hash);
			}
			m_TilesetInfoList.push_back(info);
		}
		else
		{
			std::cerr << "Failed to parse tileset file: " << tsxFilename << std::endl;
		}
	}
}

TmxParser::TilesetInfo* TmxParser::InfoForGid(int32 gid)
{
	for (TilesetInfo& info : m_TilesetInfoList)
	{
		if (gid >= info.m_FirstGid && gid <= info.m_LastGid)
		{
			return &info;
		}
	}
	return nullptr;
}

int32 TmxParser::GidToTileId(int32 gid)
{
	if (TilesetInfo* info = InfoForGid(gid))
	{
		return gid - info->m_FirstGid;
	}
	return -1;
}

NameHash TmxParser::GidToObjectName(int32 gid)
{
	if (TilesetInfo* info = InfoForGid(gid))
	{
		if (NameHash* name = Util::Find(info->m_GidToName, gid))
		{
			return *name;
		}
	}
	return "Invalid";
}
