#include "HealthComponent.h"

#include <algorithm>
#include "Gameplay/Components/SpriteComponent.h"
#include "Gameplay/GameObject.h"

TypeInfo const HealthComponent::s_TypeInfo = TypeInfo("HealthComponent", { "Component" });

void HealthComponent::Tick(float deltaTime)
{
	// TODO use a proper timer with timer manager
	if (m_HitFlashTimer > 0.0f)
	{
		m_HitFlashTimer -= deltaTime;
		if (m_HitFlashTimer <= 0)
		{
			StopHitFlash();
		}
	}
}

void HealthComponent::Heal(float amount)
{
	m_Health = std::min(m_MaxHealth, m_Health + amount);
}

void HealthComponent::ApplyDamage(float amount)
{
	m_Health = std::max(0.0f, m_Health - amount);
	StartHitFlash();
}


void HealthComponent::StartHitFlash()
{
	std::vector<SpriteComponent*> sprites = GetOwner()->FindAllComponentsByType<SpriteComponent>();
	for (SpriteComponent* sprite : sprites)
	{
		sprite->SetHitFlash(true);
	}

	m_HitFlashTimer = c_HitFlashTime;
}

void HealthComponent::StopHitFlash()
{
	std::vector<SpriteComponent*> sprites = GetOwner()->FindAllComponentsByType<SpriteComponent>();
	for (SpriteComponent* sprite : sprites)
	{
		sprite->SetHitFlash(false);
	}
}
