#include "InputEventManager.h"

MulticastEvent<>& InputEventManager::GetKeyPressedEvent(sf::Keyboard::Key key)
{
	return m_KeyPressedEventList[key];
}

MulticastEvent<>& InputEventManager::GetKeyReleasedEvent(sf::Keyboard::Key key)
{
	return m_KeyReleasedEventList[key];
}

void InputEventManager::HandleKeyPressed(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_LastEvent = keyEvent;
		m_KeyPressedEventList[keyEvent.code].Broadcast();
	}
}

void InputEventManager::HandleKeyReleased(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_LastEvent = keyEvent;
		m_KeyReleasedEventList[keyEvent.code].Broadcast();
	}
}

sf::Event::KeyEvent const& InputEventManager::GetEventDetails()
{
	return m_LastEvent;
}
