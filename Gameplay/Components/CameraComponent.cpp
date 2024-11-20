#include "CameraComponent.h"

#include <algorithm>
#include "Gameplay/GameObject.h"
#include "Gameplay/World.h"
#include "SFML/Graphics/View.hpp"

TypeInfo const CameraComponent::s_TypeInfo = TypeInfo("CameraComponent", {"Component"});

void CameraComponent::Tick(float deltaTime)
{
	// Interpolate to target
	if (m_Offset.x < m_TargetOffset.x)
	{
		m_Offset.x = std::min(m_TargetOffset.x, m_Offset.x + m_Speed);
	}
	else if (m_Offset.x > m_TargetOffset.x)
	{
		m_Offset.x = std::max(m_TargetOffset.x, m_Offset.x - m_Speed);
	}

	if (m_Offset.y < m_TargetOffset.y)
	{
		m_Offset.y = std::min(m_TargetOffset.y, m_Offset.y + m_Speed);
	}
	else if (m_Offset.y > m_TargetOffset.y)
	{
		m_Offset.y = std::max(m_TargetOffset.y, m_Offset.y - m_Speed);
	}

	// Update view
	IntVec centre = GetOwner()->GetCentre() + m_Offset;
	sf::View view(ToFVec(centre), ToFVec(m_ViewSize));
	GetWorld()->SetMainView(view);
}
