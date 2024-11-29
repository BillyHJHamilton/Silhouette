#pragma once

#include <string>
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector3.hpp"

namespace Util
{
	// Expects format "#aarrggbb"
	sf::Color ParseTiledColor(const std::string& tiledColour);

	inline sf::Vector3f ColourToFVec3(const sf::Color& colour)
	{
		return sf::Vector3f(
			static_cast<float>(colour.r)/255.0f,
			static_cast<float>(colour.g) / 255.0f,
			static_cast<float>(colour.b) / 255.0f);
	}
}
