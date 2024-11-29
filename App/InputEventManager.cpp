#include "InputEventManager.h"

MulticastEvent<>& InputEventManager::GetKeyPressedEvent(sf::Keyboard::Key key)
{
	return m_KeyPressedEventList[key];
}

MulticastEvent<>& InputEventManager::GetKeyReleasedEvent(sf::Keyboard::Key key)
{
	return m_KeyReleasedEventList[key];
}

MulticastEvent<>& InputEventManager::GetButtonPressedEvent(uint32 button)
{
	return m_ButtonPressedEventList[button];
}

MulticastEvent<>& InputEventManager::GetButtonReleasedEvent(uint32 button)
{
	return m_ButtonReleasedEventList[button];
}

MulticastEvent<uint32>& InputEventManager::GetAnyButtonPressedEvent()
{
	return m_AnyButtonPressedEvent;
}

MulticastEvent<uint32>& InputEventManager::GetAnyButtonReleasedEvent()
{
	return m_AnyButtonReleasedEvent;
}

void InputEventManager::HandleKeyPressed(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_LastKeyEvent = keyEvent;
		m_KeyPressedEventList[keyEvent.code].Broadcast();
	}
}

void InputEventManager::HandleKeyReleased(sf::Event::KeyEvent const& keyEvent)
{
	if (keyEvent.code > sf::Keyboard::Key::Unknown &&
		keyEvent.code < sf::Keyboard::Key::KeyCount)
	{
		m_LastKeyEvent = keyEvent;
		m_KeyReleasedEventList[keyEvent.code].Broadcast();
	}
}

void InputEventManager::HandleButtonPressed(sf::Event::JoystickButtonEvent const& buttonEvent)
{
	if (buttonEvent.button >= 0 &&
		buttonEvent.button < sf::Joystick::ButtonCount)
	{
		m_ButtonPressedEventList[buttonEvent.button].Broadcast();
	}
}

void InputEventManager::HandleButtonReleased(sf::Event::JoystickButtonEvent const& buttonEvent)
{
	if (buttonEvent.button >= 0 &&
		buttonEvent.button < sf::Joystick::ButtonCount)
	{
		m_ButtonReleasedEventList[buttonEvent.button].Broadcast();
	}
}

sf::Event::KeyEvent const& InputEventManager::GetEventDetails()
{
	return m_LastKeyEvent;
}
