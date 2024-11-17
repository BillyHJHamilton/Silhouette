#include "GameApp.h"

#include "App/AssetManager.h"
#include "Gameplay/World.h"
#include "SFML/Window/Event.hpp"
#include "Util/Random.h"

GameApp* GameApp::s_Instance = nullptr;

GameApp::~GameApp()
{
	// Add any additional deinitialization here.
}

GameApp& GameApp::Get()
{
	assert(s_Instance != nullptr);
	return *s_Instance;
}

AssetManager& GameApp::GetAssetManager()
{
	assert(s_Instance != nullptr);
	return Get().m_AssetManager;
}

InputEventManager& GameApp::GetInputEventManager()
{
	assert(s_Instance != nullptr);
	return Get().m_InputEventManager;
}

ShaderManager& GameApp::GetShaderManager()
{
	assert(s_Instance != nullptr);
	return Get().m_ShaderManager;
}

double GameApp::GetClockTime() const
{
	return m_Clock.getElapsedTime().asMicroseconds() / 1'000'000.0;
}

void GameApp::Run()
{
	assert(s_Instance == nullptr);
	s_Instance = this;

#if UNIT_TESTS
	StartupEngineTests();
#endif UNIT_TESTS

	StartupLoadAssets();
	StartupInit();

	CreateWindow();

	AppLoop();

	ShutdownCleanup();
}

void GameApp::StartupLoadAssets()
{
	m_AssetManager.LoadTexture("Tileset", "Resources/Textures/MiniTileset.png");
	m_ShaderManager.LoadShaders();
}

void GameApp::StartupInit()
{
	Random::SeedGenerator();

	// TODO maybe handle this pointer better
	m_CurrentWorld = new World();
	m_CurrentWorld->Init();
}

void GameApp::ShutdownCleanup()
{
	delete m_CurrentWorld;
}

void GameApp::CreateWindow()
{
	// TODO Provide a way to configure the window.
	m_MainWindow.create(sf::VideoMode(640, 480), "Silhouette");

	m_MainWindow.setKeyRepeatEnabled(false);

	m_Clock.restart();
}

void GameApp::AppLoop()
{
	double frameStart = GetClockTime();
	while (m_MainWindow.isOpen())
	{
		float fixedFrameDuration = 1.0f / static_cast<float>(m_FixedFPS);
		double frameEnd = frameStart + fixedFrameDuration;

		AppHandleEvents();

		AppTick(fixedFrameDuration);

		do
		{
			AppDraw();
		}
		while (GetClockTime() < frameEnd);

		frameStart = frameEnd;
	}
}

void GameApp::AppHandleEvents()
{
	sf::Event nextEvent;
	while (m_MainWindow.pollEvent(nextEvent))
	{
		switch (nextEvent.type)
		{
			case sf::Event::Closed:
				m_MainWindow.close();
				break;

			case sf::Event::KeyPressed:
				m_InputEventManager.HandleKeyPressed(nextEvent.key);
				break;

			case sf::Event::KeyReleased:
				m_InputEventManager.HandleKeyReleased(nextEvent.key);
				break;
		}
	}
}

void GameApp::AppTick(float deltaTime)
{
	if (m_CurrentWorld != nullptr)
	{
		m_CurrentWorld->Tick(deltaTime);
	}
}

void GameApp::AppDraw()
{
	m_MainWindow.clear();
	if (m_CurrentWorld != nullptr)
	{
		m_CurrentWorld->Draw(m_MainWindow);
	}
	m_MainWindow.display();
}

#if UNIT_TESTS
void GameApp::StartupEngineTests()
{
	TestReferences();
	TestEvents();
	NameHash::UnitTest();
	//GameObject::UnitTest();
}
#endif

AppTestWrapper::~AppTestWrapper()
{
#if DEBUG_MEMORY
	//GameObject::CheckMemoryReleased();
	//GameSystem::CheckMemoryReleased();
	//Component::CheckMemoryReleased();
	RefControlBlock::CheckMemoryReleased();
#endif
}
