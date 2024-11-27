#include "PlatformerApp.h"
#include "Gameplay/WorldGrid/LoadWorld.h"

/*static*/ PlatformerApp& PlatformerApp::Get()
{
	return static_cast<PlatformerApp&>(GameApp::Get());
}

void PlatformerApp::StartupLoadAssets()
{
	m_AssetManager.LoadTexture("Tileset", "Resources/Textures/Tiles1.png");
	m_AssetManager.LoadTexture("Player", "Resources/Textures/BalletSheet.png");
	m_AssetManager.LoadTexture("Objects1x1", "Resources/Textures/Objects1x1.png");
	m_ShaderManager.LoadShaders();
}

void PlatformerApp::StartupInit()
{
	{
		TmxParser tmxParser;
		tmxParser.LoadMapFromTmx("Resources/Maps/Map2.tmx", "Resources/Maps/", m_World);
	}
	m_World.Init();
	m_CurrentWorld = &m_World;

	m_InputEventManager.GetKeyPressedEvent(sf::Keyboard::Escape).AddDelegate(this, &PlatformerApp::OnEscapePressed);
	m_InputEventManager.GetKeyPressedEvent(sf::Keyboard::F11).AddDelegate(this, &PlatformerApp::OnF11Pressed);
}

void PlatformerApp::ShutdownCleanup()
{
	m_InputEventManager.GetKeyPressedEvent(sf::Keyboard::Escape).RemoveDelegatesForReceiver(this);
}

void PlatformerApp::OnEscapePressed()
{
	m_MainWindow.close();
}

void PlatformerApp::OnF11Pressed()
{
	ToggleFullscreen();
}
