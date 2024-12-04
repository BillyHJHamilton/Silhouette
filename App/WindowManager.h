#pragma once

#include "Util/Vec2.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/View.hpp"

class WindowManager
{
public:
	static WindowManager& Get();

	void SetNativeResolution(IntVec resolution);
	void SetDefaultWindowScale(float defaultWindowScale);

	void CreateWindow();
	void CreateWindowFullscreen();

	void ToggleFullscreen();

	sf::RenderWindow& GetMainWindow();

	float GetScreenRatio() const;
	IntVec GetNativeResolution() const;

	// Returns a view with top-left at 0,0 and a size equal to the native resolution.
	// Appropriate for drawing UI elements.
	sf::View GetNativeView() const;

protected:
	sf::RenderWindow m_MainWindow;

	IntVec m_NativeResolution = {384,288};
	float m_DefaultWindowScale = 2.0f;
	bool m_IsFullscreen;
};
