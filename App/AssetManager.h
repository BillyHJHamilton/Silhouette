#pragma once

#include <unordered_map>
#include "Core/NameHash.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"

class AssetManager
{
public:
	// Fonts

	void LoadFont(NameHash fontName, std::string filename);
	const sf::Font* FindFont(NameHash fontName);

	// Textures

	struct TextureLoadOptions
	{
		// Pixels that exactly match the provided colour will be changed to transparent.
		sf::Color m_TransparentColour = sf::Color::Transparent;
	};

	void LoadTexture(NameHash textureName, std::string filename,
		TextureLoadOptions options = TextureLoadOptions());

	const sf::Texture* FindTexture(NameHash textureName);

private:
	std::unordered_map<NameHash,sf::Font> m_FontMap;
	std::unordered_map<NameHash,sf::Texture> m_TextureMap;
};
