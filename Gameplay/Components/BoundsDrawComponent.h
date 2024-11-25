#pragma once

#include "Gameplay/Component.h"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

// Debug component.  Attach to an object to draw its bounding box.
class BoundsDrawComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(BoundsDrawComponent)

	virtual void OnAddedToObject(GameObject& newOwner) override;
	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const override;

private:
	sf::RectangleShape m_Rectangle;
};
