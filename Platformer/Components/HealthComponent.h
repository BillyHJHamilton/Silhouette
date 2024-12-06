#pragma once

#include <vector>
#include "Gameplay/Component.h"

class SpriteComponent;

// Component for things that have hit points and can take damage.
// Also applies hit flash to the sprite.
class HealthComponent : public Component
{
public:
	MACRO_DeclareTypeInfo(HealthComponent)

	HealthComponent(float maxHealth) : m_MaxHealth(maxHealth), m_Health(maxHealth) {}

	virtual void Tick(float deltaTime) override;

	float GetHealth() const { return m_Health; }
	float GetMaxHealth() const { return m_MaxHealth; }
	float GetHealthPercent() const { return m_Health / m_MaxHealth; }

	bool IsDead() const { return m_Health <= 0.0f; }

	void SetMaxHealth(float maxHealth) { m_MaxHealth = maxHealth; }
	void SetHealth(float health) { m_Health = health; }
	void SetHealthToMax() { m_Health = m_MaxHealth; }

	void Heal(float amount);
	void ApplyDamage(float amount);

	// Todo: Invincibility frames?  Perhaps against specific instigator only?

protected:
	void StartHitFlash();
	void StopHitFlash();

	float m_MaxHealth = 1.0f;
	float m_Health = 1.0f;

	float m_HitFlashTimer = 0.0f;
	static constexpr float c_HitFlashTime = 0.6f;
	// TODO: Use a timer manager instead of this busy timer.
};
