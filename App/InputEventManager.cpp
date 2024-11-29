#include "InputEventManager.h"

#include <cmath>

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
		m_LastJoystickId = buttonEvent.joystickId;
		m_ButtonPressedEventList[buttonEvent.button].Broadcast();
		m_AnyButtonPressedEvent.Broadcast(buttonEvent.button);
	}
}

void InputEventManager::HandleButtonReleased(sf::Event::JoystickButtonEvent const& buttonEvent)
{
	if (buttonEvent.button >= 0 &&
		buttonEvent.button < sf::Joystick::ButtonCount)
	{
		m_ButtonReleasedEventList[buttonEvent.button].Broadcast();
		m_AnyButtonReleasedEvent.Broadcast(buttonEvent.button);
	}
}

void InputEventManager::HandleJoystickMoved(sf::Event::JoystickMoveEvent const& moveEvent)
{
	if (std::abs(moveEvent.position > 1.0f))
	{
		m_LastJoystickId = moveEvent.joystickId;
	}
}

float InputEventManager::GetAxis(sf::Joystick::Axis axis) const
{
	return sf::Joystick::getAxisPosition(m_LastJoystickId, axis);
}

sf::Event::KeyEvent const& InputEventManager::GetEventDetails()
{
	return m_LastKeyEvent;
}
