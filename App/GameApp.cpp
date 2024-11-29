#include "GameApp.h"

#include "App/AssetManager.h"
#include "App/PerfTimer.h"
#include "Gameplay/Component.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/GameSystem.h"
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

#if _DEBUG
	CreateWindow();
#else
	CreateWindowFullscreen();
#endif

	AppLoop();

	ShutdownCleanup();
}

void GameApp::StartupLoadAssets()
{
}

void GameApp::StartupInit()
{
	Random::SeedGenerator();
}

void GameApp::ShutdownCleanup()
{
}

void GameApp::CreateWindow()
{
	m_MainWindow.create(sf::VideoMode(640, 480), "Silhouette");
	m_MainWindow.setKeyRepeatEnabled(false);
	m_MainWindow.setVerticalSyncEnabled(true);
	m_MainWindow.setFramerateLimit(0);
	m_IsFullscreen = false;
}

void GameApp::CreateWindowFullscreen()
{
	m_MainWindow.create(sf::VideoMode::getDesktopMode(), "Silhouette", sf::Style::None);
	m_MainWindow.setKeyRepeatEnabled(false);
	m_MainWindow.setVerticalSyncEnabled(true);
	m_MainWindow.setFramerateLimit(0);
	m_IsFullscreen = true;
}

void GameApp::ToggleFullscreen()
{
	if (!m_IsFullscreen)
	{
		CreateWindowFullscreen();
	}
	else
	{
		CreateWindow();
	}
}

float GameApp::GetScreenRatio()
{
	sf::Vector2u screenSize = m_MainWindow.getSize();
	return static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y);
}

void GameApp::AppLoop()
{
	m_Clock.restart();

	#if PERF_TEST
		int32 numDraws = 0;
	#endif

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

			#if PERF_TEST
				++numDraws;
			#endif
		}
		while (GetClockTime() < frameEnd);

		frameStart = frameEnd;
	}

	#if PERF_TEST
		std::cout << std::endl << "Average framerate: " << numDraws/frameStart << " FPS" << std::endl;
	#endif
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
	{
		PerfTimer timer("VSync");
		m_ShaderManager.ClearLights();
		m_ShaderManager.ClearNormalTransform();
	}

	if (m_CurrentWorld != nullptr)
	{
		m_CurrentWorld->Tick(deltaTime);
	}
}

void GameApp::AppDraw()
{
	PerfTimer timer(__FUNCTION__);

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
#if PERF_TEST
	PerfTimer::PrintAll();
#endif

#if DEBUG_MEMORY
	GameObject::CheckMemoryReleased();
	GameSystem::CheckMemoryReleased();
	Component::CheckMemoryReleased();
	RefControlBlock::CheckMemoryReleased();
#endif
}
