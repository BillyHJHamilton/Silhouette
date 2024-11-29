#pragma once

#include "Gameplay/Component.h"
#include "Gameplay/RenderLayer.h"
#include "SFML/Graphics/Text.hpp"

class TextComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(TextComponent)

	void SetVisible(bool visibility) { m_Visible = visibility; };
	void SetLayer(RenderLayer::Enum layer) { m_RenderLayer = layer; }

	void SetFont(NameHash fontName);

	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const override;

	// Public to allow changing local transform, colour, size, etc.
	sf::Text m_Text;

private:
	bool m_Visible = true;
	RenderLayer::Enum m_RenderLayer = RenderLayer::FrontEffects_Unlit;
};
