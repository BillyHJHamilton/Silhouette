#include "InputEventManager.h"

MulticastEvent<sf::Event::KeyEvent const&>& InputEventManager::GetKeyPressedEvent(sf::Keyboard::Key key)
{
	return m_KeyPressedEventList[key];
}

MulticastEvent<sf::Event::KeyEvent const&>& InputEventManager::GetKeyReleasedEvent(sf::Keyboard::Key key)
{
	return m_KeyReleasedEventList[key];
}

void InputEventManager::HandleKeyPressed(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_KeyPressedEventList[keyEvent.code].Broadcast(keyEvent);
	}
}

void InputEventManager::HandleKeyReleased(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_KeyReleasedEventList[keyEvent.code].Broadcast(keyEvent);
	}
}
