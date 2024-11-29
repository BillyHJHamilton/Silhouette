#pragma once

#include <array>
#include <vector>
#include "Core/BasicTypes.h"
#include "Gameplay/RenderLayer.h"
#include "SFML/Graphics/RenderStates.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "Util/Vec2.h"

namespace sf
{
	class Drawable;
	class RenderTarget;
}

// Used by World to gather renderable objects into layers before drawing.
class RenderManager
{
public:

	// Request to draw an object in the next draw event.
	// Note: Raw pointer is stored, so the object must not be destroyed before DrawAll is called.
	void AddDrawable(RenderLayer::Enum layer, const sf::Drawable* drawable,
		sf::Transform renderTransform = sf::Transform::Identity,
		float normalRotation = 0.0f, FVec normalScale = FVec(1.0f,1.0f));

	// Draw all the enqueued objects.
	void DrawAll(sf::RenderTarget& target);

protected:
	struct RenderInfo
	{
		const sf::Drawable* m_Drawable;
		sf::Transform m_RenderTransform;

		// Only used for lit layers:
		float m_NormalRotation;
		FVec m_NormalScale;
	};

	std::array<std::vector<RenderInfo>, RenderLayer::Count> m_RenderList;
};
