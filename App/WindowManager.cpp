#include "WindowManager.h"
#include "GameApp.h"
#include "Util/Rect.h"

WindowManager& WindowManager::Get()
{
	return GameApp::GetWindowManager();
}

void WindowManager::CreateWindow()
{
	IntVec windowSize = RoundToIntVec(m_DefaultWindowScale * ToFVec(m_NativeResolution));
	m_MainWindow.create(sf::VideoMode(windowSize.x, windowSize.y), "Silhouette");
	m_MainWindow.setKeyRepeatEnabled(false);
	m_MainWindow.setVerticalSyncEnabled(true);
	m_MainWindow.setFramerateLimit(0);
	m_IsFullscreen = false;
}

void WindowManager::CreateWindowFullscreen()
{
	m_MainWindow.create(sf::VideoMode::getDesktopMode(), "Silhouette", sf::Style::None);
	m_MainWindow.setKeyRepeatEnabled(false);
	m_MainWindow.setVerticalSyncEnabled(true);
	m_MainWindow.setFramerateLimit(0);
	m_IsFullscreen = true;
}

void WindowManager::SetNativeResolution(IntVec resolution)
{
	assert(resolution.x > 0);
	assert(resolution.y > 0);
	m_NativeResolution = resolution;
}

void WindowManager::SetDefaultWindowScale(float defaultWindowScale)
{
	m_DefaultWindowScale = defaultWindowScale;
}

void WindowManager::ToggleFullscreen()
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

sf::RenderWindow& WindowManager::GetMainWindow()
{
	return m_MainWindow;
}

float WindowManager::GetScreenRatio() const
{
	sf::Vector2u screenSize = m_MainWindow.getSize();
	return static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y);
}

IntVec WindowManager::GetNativeResolution() const
{
	return m_NativeResolution;
}

sf::View WindowManager::GetNativeView() const
{
	return sf::View(FRect({0.0f,0.0f}, ToFVec(m_NativeResolution)));
}
