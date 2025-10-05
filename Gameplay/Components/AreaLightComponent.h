#pragma once

#include "Gameplay/Component.h"
#include "SFML/System/Vector3.hpp"
#include "Util/Vec2.h"
#include "Util/Rect.h"

// Applies an area light inside the owner's bounding box.
class AreaLightComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(AreaLightComponent)

	virtual bool ShouldTickWhenPaused() const override { return true; }
	virtual void Tick(float deltaTime) override;

	void SetColour(sf::Vector3f colour) { m_Colour = colour; }
	void SetColour(float r, float g, float b) { m_Colour = { r,g,b }; }
	void SetBorderSize(IntVec borderSize) { m_BorderSize = borderSize; }
	void SetLightVector(FVec lightVector) { m_LightVector = lightVector; }
	void SetBrightness(float brightness) { m_Brightness = brightness; }
	void SetEnabled(bool enabled) { m_Enabled = enabled; }
	void ToggleEnabled() { m_Enabled = !m_Enabled; }

private:
	sf::Vector3f m_Colour = { 1.0f, 1.0f, 1.0f };
	IntVec m_BorderSize = {1, 1};
	FVec m_LightVector = {0.0f, 0.0f};
	float m_Brightness = 1.0f;
	bool m_Enabled = true;
};
