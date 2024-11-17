#pragma once

#include "Core/BasicTypes.h"
#include "SFML/Graphics/Texture.hpp"

using namespace sf;

struct Tileset
{
	const Texture* m_Texture = nullptr;
	int32 m_TilesPerRow = 128;
};
