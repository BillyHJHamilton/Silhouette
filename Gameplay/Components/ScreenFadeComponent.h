#pragma once

#include "Gameplay/Component.h"
#include "SFML/Graphics/RectangleShape.hpp"

class ScreenFadeComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(ScreenFadeComponent)

	ScreenFadeComponent(sf::Color colour = sf::Color::Black);

	virtual void OnAddedToObject(GameObject& newOwner) override;
	virtual void Tick(float deltaTime) override;
	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& ObjectTransform) const override;

	void SetColour(sf::Color colour);

	void SetFadePercent(float percent) { m_FadePercent = percent; }
	void SetFadeInSpeed(float speed) { m_FadeInSpeed = speed; }
	void SetFadeOutSpeed(float speed) { m_FadeOutSpeed = speed; }

	enum class Direction
	{
		FadeIn,
		FadeOut,
		Pause
	};
	void SetFadeDirection(Direction direction) { m_Direction = direction; }

	void StartFadeIn(float fadeTime, bool fromMax);
	void StartFadeOut(float fadeTime, bool fromZero);

protected:
	void UpdateColour();

	sf::RectangleShape m_Rectangle;
	sf::Color m_Colour;

	Direction m_Direction;

	float m_FadePercent = 0.0f;
	float m_FadeOutSpeed = 0.0f; // Speed when fading to solid colour.
	float m_FadeInSpeed = 0.0f; // Speed when fading back in to normal scene.
};
