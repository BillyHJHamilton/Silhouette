#pragma once

#include "App/AssetManager.h"
#include "App/InputEventManager.h"
#include "App/ShaderManager.h"

#include "SFML/Graphics/RenderWindow.hpp"

class World;

// Used to run memory tests after GameApp destructor.
class AppTestWrapper
{
public:
	virtual ~AppTestWrapper();
};

class GameApp : public AppTestWrapper
{
public:
	virtual ~GameApp();

	static GameApp& Get();
	static AssetManager& GetAssetManager();
	static InputEventManager& GetInputEventManager();
	static ShaderManager& GetShaderManager();

	// Returns seconds since the game app started.
	double GetClockTime() const;

	void Run();

	void ToggleFullscreen();
	float GetScreenRatio();

protected:
	virtual void StartupLoadAssets();
	virtual void StartupInit();

	virtual void ShutdownCleanup();

	virtual void CreateWindow();
	virtual void CreateWindowFullscreen();

	virtual void AppLoop();

	void AppHandleEvents();
	void AppTick(float deltaTime);
	void AppDraw();

	sf::RenderWindow m_MainWindow;
	sf::Clock m_Clock;

	bool m_IsFullscreen;
	
	float m_FixedFPS = 60.0f;
	//float m_FixedFPS = 10.0f;
	float m_LastTick = 0.0f;

	AssetManager m_AssetManager;
	InputEventManager m_InputEventManager;
	ShaderManager m_ShaderManager;
	World* m_CurrentWorld = nullptr;

private:
#if UNIT_TESTS
	void StartupEngineTests();
#endif

	static GameApp* s_Instance;
};
