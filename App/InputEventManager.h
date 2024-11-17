#pragma once

#include <array>
#include "Core/Event.h"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Event.hpp"

class InputEventManager
{
public:
	// Get the key event, so that you can add a handler.
	MulticastEvent<sf::Event::KeyEvent const&>& GetKeyPressedEvent(sf::Keyboard::Key key);
	MulticastEvent<sf::Event::KeyEvent const&>& GetKeyReleasedEvent(sf::Keyboard::Key key);

	void HandleKeyPressed(sf::Event::KeyEvent const& keyEvent);
	void HandleKeyReleased(sf::Event::KeyEvent const& keyEvent);

private:
	std::array<MulticastEvent<sf::Event::KeyEvent const&>,sf::Keyboard::KeyCount> m_KeyPressedEventList;
	std::array<MulticastEvent<sf::Event::KeyEvent const&>,sf::Keyboard::KeyCount> m_KeyReleasedEventList;
};
