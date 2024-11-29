#pragma once

#include "Gameplay/Component.h"
#include "SFML/System/Vector3.hpp"
#include "Util/Vec2.h"

class PointLightComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(PointLightComponent)

	virtual void Tick(float deltaTime) override;

	void SetColour(sf::Vector3f colour) { m_Colour = colour; }
	void SetColour(float r, float g, float b) { m_Colour = {r,g,b}; }
	void SetBrightness(float brightness) { m_Brightness = brightness; }
	void SetRadius(float radius) { m_Radius = radius; }
	void SetOffset(IntVec vec) { m_Offset = vec; }
	void SetEnabled(bool enabled) { m_Enabled = enabled; }
	void ToggleEnabled() { m_Enabled = !m_Enabled; }

private:
	sf::Vector3f m_Colour = {1.0f, 1.0f, 1.0f};
	IntVec m_Offset = {0,0};
	float m_Brightness = 1.0f;
	float m_Radius = 300.0f;
	bool m_Enabled = true;
};
