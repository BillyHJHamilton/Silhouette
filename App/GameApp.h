#pragma once

#include <memory>
#include "Core/Core.h"
#include "SFML/System/Clock.hpp"

class AssetManager;
class InputEventManager;
class ShaderManager;
class WindowManager;
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
	GameApp();
	virtual ~GameApp();

	static GameApp& Get();
	static AssetManager& GetAssetManager();
	static InputEventManager& GetInputEventManager();
	static ShaderManager& GetShaderManager();
	static WindowManager& GetWindowManager();

	// Returns seconds since the game app started.
	double GetClockTime() const;

	void Run();

protected:
	virtual void StartupLoadAssets();
	virtual void StartupInit();

	virtual void ShutdownCleanup();

	virtual void AppLoop();

	void AppHandleEvents();
	void AppTick(float deltaTime);
	void AppDraw();

	sf::Clock m_Clock;

	float m_FixedFPS = 60.0f;
	float m_LastTick = 0.0f;

	std::unique_ptr<AssetManager> m_AssetManager;
	std::unique_ptr<InputEventManager> m_InputEventManager;
	std::unique_ptr<ShaderManager> m_ShaderManager;
	std::unique_ptr<WindowManager> m_WindowManager;

	World* m_CurrentWorld = nullptr;

private:
#if UNIT_TESTS
	void StartupEngineTests();
#endif

	static GameApp* s_Instance;
};
