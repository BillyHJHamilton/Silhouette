#include "HealthbarComponent.h"

#include "Gameplay/RenderManager.h"
#include "Gameplay/GameObject.h"
#include "Platformer/Components/HealthComponent.h"

TypeInfo const HealthbarComponent::s_TypeInfo = TypeInfo("HealthbarComponent", { "Component" });

void HealthbarComponent::OnAddedToObject(GameObject& newOwner)
{
	m_Background.setPosition(FVec(9.0f, 9.0f));
	m_Foreground.setPosition(FVec(9.0f, 9.0f));

	m_Background.setSize(FVec(60.0f, 6.0f));
	m_Foreground.setSize(FVec(60.0f, 6.0f));

	m_Background.setFillColor(sf::Color::Black);
	m_Background.setOutlineColor(sf::Color(64u, 64u, 64u));
	m_Background.setOutlineThickness(1);

	m_Foreground.setFillColor(sf::Color(96u, 0u, 12u));
}

void HealthbarComponent::Tick(float deltaTime)
{
	if (HealthComponent* healthComponent = GetOwner()->FindComponent<HealthComponent>())
	{
		const FVec backSize = m_Background.getSize();
		const float hpPercent = healthComponent->GetHealthPercent();
		const float newWidth = backSize.x * hpPercent;
		m_Foreground.setSize({newWidth, backSize.y});
	}
}

void HealthbarComponent::GatherDraw(RenderManager& renderManager, const sf::Transform& ObjectTransform) const
{
	renderManager.AddDrawable(RenderLayer::UI_Unlit, &m_Background);
	renderManager.AddDrawable(RenderLayer::UI_Unlit, &m_Foreground);
}
