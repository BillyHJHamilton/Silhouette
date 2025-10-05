#pragma once

#include "SFML/Graphics/Rect.hpp"

using IntRect = sf::IntRect;
using FRect = sf::FloatRect;

namespace Util
{
	// Adds the given amount to each side of a rectangle.
	IntRect PadRect(IntRect rect, int paddingX, int paddingY);
	FRect PadRect(FRect rect, float paddingX, float paddingY);
}
