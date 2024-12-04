#include "AssetManager.h"
#include "App/GameApp.h"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"

AssetManager& AssetManager::Get()
{
	return GameApp::GetAssetManager();
}

void AssetManager::LoadFont(NameHash fontName, std::string filename)
{
	if (m_FontMap.find(fontName) != m_FontMap.end())
	{
		#if DEBUG_NAME_HASHING
			std::cerr << "Error: Can't load font to " << fontName.GetNameString()
				<< " because that name is already in use." << std::endl;
		#endif

		return;
	}

	auto emplaceResult = m_FontMap.emplace(fontName, sf::Font());
	if (!emplaceResult.second)
	{
		std::cerr << "Error: Failed to create font resource." << std::endl;
		return;
	}

	sf::Font& newFont = emplaceResult.first->second;

	bool const bLoaded = newFont.loadFromFile(filename);
	if (!bLoaded)
	{
		m_FontMap.erase(fontName);
		return;
	}
}

const sf::Font* AssetManager::FindFont(NameHash fontName)
{
	auto Itr = m_FontMap.find(fontName);
	if (Itr == m_FontMap.end())
	{
		return nullptr;
	}
	else
	{
		return &Itr->second;
	}
}

void AssetManager::LoadTexture(NameHash textureName, std::string filename,
	TextureLoadOptions options)
{
	if (m_TextureMap.find(textureName) != m_TextureMap.end())
	{
		#if DEBUG_NAME_HASHING
			std::cerr << "Error: Can't load texture to " << textureName.GetNameString()
				<< " because that name is already in use." << std::endl;
		#endif

		return;
	}

	m_TextureMap.emplace(textureName, sf::Texture());

	// Load the image from a file.
	sf::Image tempImage;
	bool const bLoaded = tempImage.loadFromFile(filename);
	if (!bLoaded)
	{
		m_TextureMap.erase(textureName);
		return;
	}

	// If a transparent colour was given, mask out those pixels.
	if (options.m_TransparentColour != sf::Color::Transparent)
	{
		tempImage.createMaskFromColor(options.m_TransparentColour, 0u);
	}

	// Create texture from the loaded image.
	m_TextureMap[textureName].loadFromImage(tempImage);
}

const sf::Texture* AssetManager::FindTexture(NameHash textureName)
{
	auto Itr = m_TextureMap.find(textureName);
	if (Itr == m_TextureMap.end())
	{
		return nullptr;
	}
	else
	{
		return &Itr->second;
	}
}
