#pragma once

#include "Gameplay/GameObject.h"

class PointLightComponent;
class SpriteComponent;

class StrobeLight : public GameObject
{
public:
	StrobeLight(IntVec position);

	virtual void Init() override;
	virtual void Tick(float deltaTime) override;

protected:
	static constexpr float c_BaseStrobePeriod = 5.0f;

	float m_StrobePeriod = c_BaseStrobePeriod;
	float m_StrobeOffset = 0.0f;

	PointLightComponent* m_PointLight = nullptr;
	SpriteComponent* m_UnlitSprite = nullptr;
};
