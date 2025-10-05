#include "PlatformerApp.h"

#include "App/AssetManager.h"
#include "App/InputEventManager.h"
#include "App/ShaderManager.h"
#include "App/WindowManager.h"
#include "Gameplay/WorldGrid/LoadWorld.h"

/*static*/ PlatformerApp& PlatformerApp::Get()
{
	return static_cast<PlatformerApp&>(GameApp::Get());
}

void PlatformerApp::StartupLoadAssets()
{
	m_AssetManager->LoadTexture("Tileset", "Resources/Textures/Tiles1.png");
	m_AssetManager->LoadTexture("Player", "Resources/Textures/BalletSheet.png");
	m_AssetManager->LoadTexture("Objects1x1", "Resources/Textures/Objects1x1.png");

	m_AssetManager->LoadFont("FixedFont", "Resources/Fonts/FSEX302.ttf");

	m_ShaderManager->LoadShaders();
}

void PlatformerApp::StartupInit()
{
	{
		TmxParser tmxParser;
		tmxParser.LoadMapFromTmx("Resources/Maps/Map2.tmx", "Resources/Maps/", m_World);
	}
	m_World.Init();
	m_CurrentWorld = &m_World;

	m_InputEventManager->GetKeyPressedEvent(sf::Keyboard::Escape).AddDelegate(this, &PlatformerApp::OnEscapePressed);
	m_InputEventManager->GetKeyPressedEvent(sf::Keyboard::F11).AddDelegate(this, &PlatformerApp::OnF11Pressed);
}

void PlatformerApp::ShutdownCleanup()
{
	m_InputEventManager->GetKeyPressedEvent(sf::Keyboard::Escape).RemoveDelegatesForReceiver(this);
	m_InputEventManager->GetKeyPressedEvent(sf::Keyboard::F11).RemoveDelegatesForReceiver(this);
}

void PlatformerApp::OnEscapePressed()
{
	m_WindowManager->GetMainWindow().close();
}

void PlatformerApp::OnF11Pressed()
{
	m_WindowManager->ToggleFullscreen();
}
