#include "GameApp.h"

#include "App/AssetManager.h"
#include "App/InputEventManager.h"
#include "App/PerfTimer.h"
#include "App/ShaderManager.h"
#include "App/WindowManager.h"
#include "Gameplay/Component.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/GameSystem.h"
#include "Gameplay/World.h"
#include "SFML/Window/Event.hpp"
#include "Util/Random.h"

GameApp* GameApp::s_Instance = nullptr;

GameApp::GameApp()
{
	m_AssetManager = std::make_unique<AssetManager>();
	m_InputEventManager = std::make_unique<InputEventManager>();
	m_ShaderManager = std::make_unique<ShaderManager>();
	m_WindowManager = std::make_unique<WindowManager>();
}

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
	assert(s_Instance && s_Instance->m_AssetManager);
	return *Get().m_AssetManager;
}

InputEventManager& GameApp::GetInputEventManager()
{
	assert(s_Instance && s_Instance->m_InputEventManager);
	return *Get().m_InputEventManager;
}

ShaderManager& GameApp::GetShaderManager()
{
	assert(s_Instance && s_Instance->m_ShaderManager);
	return *Get().m_ShaderManager;
}

WindowManager& GameApp::GetWindowManager()
{
	assert(s_Instance && s_Instance->m_WindowManager);
	return *Get().m_WindowManager;
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
	m_WindowManager->CreateWindow();
#else
	m_WindowManager->CreateWindowFullscreen();
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

void GameApp::AppLoop()
{
	m_Clock.restart();

	#if PERF_TEST
		int32 numDraws = 0;
	#endif

	double frameStart = GetClockTime();
	while (m_WindowManager->GetMainWindow().isOpen())
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
	while (m_WindowManager->GetMainWindow().pollEvent(nextEvent))
	{
		switch (nextEvent.type)
		{
			case sf::Event::Closed:
				m_WindowManager->GetMainWindow().close();
				break;

			case sf::Event::KeyPressed:
				m_InputEventManager->HandleKeyPressed(nextEvent.key);
				break;

			case sf::Event::KeyReleased:
				m_InputEventManager->HandleKeyReleased(nextEvent.key);
				break;

			case sf::Event::JoystickButtonPressed:
				m_InputEventManager->HandleButtonPressed(nextEvent.joystickButton);
				break;

			case sf::Event::JoystickButtonReleased:
				m_InputEventManager->HandleButtonReleased(nextEvent.joystickButton);
				break;

			case sf::Event::JoystickMoved:
				m_InputEventManager->HandleJoystickMoved(nextEvent.joystickMove);
				break;
		}
	}
}

void GameApp::AppTick(float deltaTime)
{
	{
		//PerfTimer timer("VSync");
		m_ShaderManager->ClearLights();
		m_ShaderManager->ClearNormalTransform();
	}

	if (m_CurrentWorld != nullptr)
	{
		m_CurrentWorld->Tick(deltaTime);
	}
}

void GameApp::AppDraw()
{
	PerfTimer timer(__FUNCTION__);

	m_WindowManager->GetMainWindow().clear();
	if (m_CurrentWorld != nullptr)
	{
		m_CurrentWorld->Draw(m_WindowManager->GetMainWindow());
	}
	m_WindowManager->GetMainWindow().display();
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
