#pragma once

#include "App/GameApp.h"
#include "Gameplay/World.h"
#include "SFML/Window/Keyboard.hpp"

class PlatformerApp : public GameApp
{
public:
	static PlatformerApp& Get();

protected:
	virtual void StartupLoadAssets() override;
	virtual void StartupInit() override;
	virtual void ShutdownCleanup() override;

private:
	void OnEscapePressed();
	void OnF11Pressed();

	World m_MainWorld;
};
