#pragma once

#include "Gameplay/Component.h"
#include "SFML/Graphics/RectangleShape.hpp"

class HealthbarComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(HealthbarComponent)

	virtual void OnAddedToObject(GameObject& newOwner) override;
	virtual void Tick(float deltaTime) override;
	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& ObjectTransform) const override;

private:
	sf::RectangleShape m_Background;
	sf::RectangleShape m_Foreground;
};

