#include "Util/ColourUtil.h"

#include "Core/Core.h"

namespace Util
{
	// Expects format "#aarrggbb"
	sf::Color ParseTiledColor(const std::string& tiledColour)
	{
		const std::string aa = tiledColour.substr(1, 2);
		const std::string rr = tiledColour.substr(3, 2);
		const std::string gg = tiledColour.substr(5, 2);
		const std::string bb = tiledColour.substr(7, 2);

		const uint8 ua = static_cast<uint8>(std::stoi(aa, 0, 16));
		const uint8 ur = static_cast<uint8>(std::stoi(rr, 0, 16));
		const uint8 ug = static_cast<uint8>(std::stoi(gg, 0, 16));
		const uint8 ub = static_cast<uint8>(std::stoi(bb, 0, 16));

		const sf::Color colour(ur, ug, ub, ua);

		//std::cout << "Converted " << tiledColour << " to ("
		//	<< (int32)colour.r << ", "
		//	<< (int32)colour.g << ", "
		//	<< (int32)colour.b << ")" << std::endl;

		return colour;
	}
}
