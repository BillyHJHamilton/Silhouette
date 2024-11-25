#include "BoundsDrawComponent.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/RenderManager.h"

TypeInfo const BoundsDrawComponent::s_TypeInfo = TypeInfo("BoundsDrawComponent", {"Component"});

void BoundsDrawComponent::OnAddedToObject(GameObject& newOwner)
{
	m_Rectangle.setFillColor(sf::Color::Transparent);
	m_Rectangle.setOutlineColor(sf::Color::Green);
	m_Rectangle.setOutlineThickness(1.0f);
	m_Rectangle.setSize(ToFVec(newOwner.GetBoundsSize()));
}

void BoundsDrawComponent::GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const
{
	renderManager.AddDrawable(RenderLayer::FrontEffects_Unlit, &m_Rectangle, objectTransform);
}
