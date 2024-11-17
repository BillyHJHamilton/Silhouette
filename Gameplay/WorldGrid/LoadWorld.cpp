#include "LoadWorld.h"

#include <fstream>
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Gameplay/WorldGrid/WorldGrid.h"
#include "SFML/System/Vector2.hpp"

void LoadTilesFromCsv(std::string filename, WorldGrid& worldGrid)
{
	std::ifstream inFile(filename);
	if (!inFile)
	{
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}
	
	sf::Vector2i tilePos(0,0);

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
