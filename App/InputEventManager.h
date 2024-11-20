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

	void HandleKeyPressed(sf::Event::KeyEvent const& keyEvent);
	void HandleKeyReleased(sf::Event::KeyEvent const& keyEvent);

	// If you want to know about modifier keys, you can retrieve them like this.
	sf::Event::KeyEvent const& GetEventDetails();

private:
	std::array<MulticastEvent<>,sf::Keyboard::KeyCount> m_KeyPressedEventList;
	std::array<MulticastEvent<>,sf::Keyboard::KeyCount> m_KeyReleasedEventList;

	sf::Event::KeyEvent m_LastEvent;
};
