#pragma once

#include "Core/BasicTypes.h"
#include "SFML/Graphics/Texture.hpp"

struct Tileset
{
	const sf::Texture* m_Texture = nullptr;
	int32 m_TilesPerRow = 128;
};
