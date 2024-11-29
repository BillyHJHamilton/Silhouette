#include "StrobeLight.h"

#include "Gameplay/Components/PointLightComponent.h"
#include "Gameplay/Components/SpriteComponent.h"
#include "Gameplay/WorldGrid/TileConstants.h"
#include "Gameplay/World.h"
#include "Util/Math.h"

StrobeLight::StrobeLight(IntVec position) :
	GameObject(IntRect(position, {c_TileWidth, c_TileHeight}))
{
}

void StrobeLight::Init()
{
	m_PointLight = EmplaceComponent<PointLightComponent>();
	m_PointLight->SetColour(1.0f, 0.7f, 0.0f);
	m_PointLight->SetBrightness(0.0f);
	m_PointLight->SetRadius(200.0f);
	m_PointLight->SetOffset({m_Bounds.width/2, m_Bounds.height/2});

	m_UnlitSprite = EmplaceComponent<SpriteComponent>("Objects1x1");
	m_UnlitSprite->SetLayer(RenderLayer::BackObjects_Unlit);

	// Some weird numbers for deterministic but irregular behaviour.
	m_StrobeOffset = static_cast<float>(m_Bounds.left) / 253.7f - static_cast<float>(m_Bounds.top) / 634.2f;
	m_StrobePeriod = c_BaseStrobePeriod * 0.8f + (0.2f * std::fmod(m_Bounds.left/551.1f + m_Bounds.top/115.6f, 1.0f));
}

void StrobeLight::Tick(float deltaTime)
{
	double worldTime = GetWorld()->GetWorldTime();
	double f = ((worldTime + m_StrobeOffset) / m_StrobePeriod) * Math::c_TwoPi;
	float bright = static_cast<float>((std::sin(f) + 1.0)/2.0);
	uint8 brightColor = std::lround(255.0f * bright);
	m_PointLight->SetBrightness(bright);
	m_UnlitSprite->m_Sprite.setColor(sf::Color(brightColor, brightColor, brightColor, 255));
}
