#include "TextComponent.h"

#include "App/GameApp.h"
#include "Gameplay/RenderManager.h"

TypeInfo const TextComponent::s_TypeInfo = TypeInfo("TextComponent", { "Component" });

void TextComponent::SetFont(NameHash fontName)
{
	const sf::Font* textFont = GameApp::GetAssetManager().FindFont(fontName);
	if (textFont != nullptr)
	{
		m_Text.setFont(*textFont);
	}
}

void TextComponent::GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const
{
	if (m_Visible && m_Text.getFont() != nullptr)
	{
		renderManager.AddDrawable(m_RenderLayer, &m_Text, objectTransform);
	}
}
