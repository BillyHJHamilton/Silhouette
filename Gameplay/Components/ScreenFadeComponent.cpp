#include "ScreenFadeComponent.h"

#include "App/WindowManager.h"
#include "Gameplay/RenderManager.h"
#include "Util/Math.h"

TypeInfo const ScreenFadeComponent::s_TypeInfo = TypeInfo("ScreenFadeComponent", { "Component" });

ScreenFadeComponent::ScreenFadeComponent(sf::Color colour) :
	m_Colour(colour)
{
	UpdateColour();
}

void ScreenFadeComponent::OnAddedToObject(GameObject& newOwner)
{
	m_Rectangle.setPosition({0.0f,0.0f});

	FVec size = ToFVec(WindowManager::Get().GetNativeResolution());
	m_Rectangle.setSize(size);
}

void ScreenFadeComponent::Tick(float deltaTime)
{
	switch (m_Direction)
	{
		case Direction::FadeIn:
		{
			m_FadePercent -= deltaTime * m_FadeInSpeed;
			if (Math::FloatLessOrEqual(m_FadePercent, 0.0f))
			{
				m_Direction = Direction::Pause;
				m_FadePercent = 0.0f;
			}
			UpdateColour();
			break;
		}

		case Direction::FadeOut:
		{
			m_FadePercent += deltaTime * m_FadeOutSpeed;
			if (Math::FloatGreaterOrEqual(m_FadePercent, 1.0f))
			{
				m_Direction = Direction::Pause;
				m_FadePercent = 1.0f;
			}
			UpdateColour();
			break;
		}
	}
}

void ScreenFadeComponent::GatherDraw(RenderManager& renderManager, const sf::Transform& ObjectTransform) const
{
	 if (!Math::FloatIsZero(m_FadePercent))
	 {
		renderManager.AddDrawable(RenderLayer::UI_ScreenFade, &m_Rectangle);
	 }
}

void ScreenFadeComponent::SetColour(sf::Color colour)
{
	m_Colour = colour;
}

void ScreenFadeComponent::StartFadeIn(float fadeTime, bool fromMax)
{
	m_Direction = Direction::FadeIn;
	m_FadeInSpeed = 1.0f / fadeTime;
	
	if (fromMax)
	{
		m_FadePercent = 1.0f;
	}
}

void ScreenFadeComponent::StartFadeOut(float fadeTime, bool fromZero)
{
	m_Direction = Direction::FadeOut;
	m_FadeOutSpeed = 1.0f / fadeTime;

	if (fromZero)
	{
		m_FadePercent = 0.0f;
	}
}

void ScreenFadeComponent::UpdateColour()
{
	sf::Color fillColour = m_Colour;
	fillColour.a = static_cast<uint8>(255.0f * m_FadePercent);
	m_Rectangle.setFillColor(fillColour);
}
