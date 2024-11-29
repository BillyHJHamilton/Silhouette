#pragma once

#include <array>
#include "Core/Event.h"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Event.hpp"

class InputEventManager
{
public:
	// Get the key event, so that you can add a handler.
	MulticastEvent<>& GetKeyPressedEvent(sf::Keyboard::Key key);
	MulticastEvent<>& GetKeyReleasedEvent(sf::Keyboard::Key key);

	// Get joystick event, so that you can add a handler.
	MulticastEvent<>& GetButtonPressedEvent(uint32 button);
	MulticastEvent<>& GetButtonReleasedEvent(uint32 button);
	MulticastEvent<uint32>& GetAnyButtonPressedEvent();
	MulticastEvent<uint32>& GetAnyButtonReleasedEvent();

	void HandleKeyPressed(sf::Event::KeyEvent const& keyEvent);
	void HandleKeyReleased(sf::Event::KeyEvent const& keyEvent);

	void HandleButtonPressed(sf::Event::JoystickButtonEvent const& buttonEvent);
	void HandleButtonReleased(sf::Event::JoystickButtonEvent const& buttonEvent);
	void HandleJoystickMoved(sf::Event::JoystickMoveEvent const& moveEvent);

	// Get axis from the most recently used joystick.
	float GetAxis(sf::Joystick::Axis axis) const;

	// If you want to know about modifier keys for the current key event, you can retrieve them like this.
	sf::Event::KeyEvent const& GetEventDetails();

private:
	std::array<MulticastEvent<>,sf::Keyboard::KeyCount> m_KeyPressedEventList;
	std::array<MulticastEvent<>,sf::Keyboard::KeyCount> m_KeyReleasedEventList;
	sf::Event::KeyEvent m_LastKeyEvent;

	std::array<MulticastEvent<>,sf::Joystick::ButtonCount> m_ButtonPressedEventList;
	std::array<MulticastEvent<>,sf::Joystick::ButtonCount> m_ButtonReleasedEventList;
	MulticastEvent<uint32> m_AnyButtonPressedEvent;
	MulticastEvent<uint32> m_AnyButtonReleasedEvent;

	uint32 m_LastJoystickId = 0;
};
