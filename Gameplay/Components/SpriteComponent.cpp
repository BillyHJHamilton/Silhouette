#include "SpriteComponent.h"

#include "App/AssetManager.h"
#include "App/ShaderManager.h"
#include "Core/TypeInfo.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/RenderManager.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/Transform.hpp"
#include "Util/Math.h"

TypeInfo const SpriteComponent::s_TypeInfo = TypeInfo("SpriteComponent", {"Component"});

SpriteComponent::SpriteComponent(NameHash textureName)
{
	const sf::Texture* spriteTexture = AssetManager::Get().FindTexture(textureName);
	if (spriteTexture != nullptr)
	{
		m_Sprite.setTexture(*spriteTexture);
	}
}

void SpriteComponent::SetupSubimages(IntVec subimageSize, IntVec firstSubimage,
	int numSubimages, int subimagesPerRow, int currentSubimage)
{
	m_SubimagesEnabled = true;
	m_SubimageSize = subimageSize;
	m_FirstSubimage = firstSubimage;
	m_NumSubimages = numSubimages;
	m_SubimagesPerRow = subimagesPerRow;
	m_CurrentSubimage = currentSubimage;

	UpdateSpriteRect();
}

void SpriteComponent::Animate(AnimationMode mode, float rate, bool loop)
{
	m_AnimationMode = mode;
	m_AnimationLoop = loop;
	m_AnimationRate = rate;
}

void SpriteComponent::Tick(float deltaTime)
{
	if (m_AnimationMode != AnimationMode::None)
	{
		if (m_AnimationMode == AnimationMode::SubimagesPerTick)
		{
			m_AnimationCounter += m_AnimationRate;
		}
		else if (m_AnimationMode == AnimationMode::SubimagesPerSecond)
		{
			m_AnimationCounter += m_AnimationRate * deltaTime;
		}
		
		int framesToAdvance = static_cast<int>(m_AnimationCounter);
		if (framesToAdvance > 0)
		{
			m_AnimationCounter -= framesToAdvance;
			AdvanceSubimage(framesToAdvance);
			UpdateSpriteRect();
		}
	}
}

void SpriteComponent::GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const
{
	if (m_Visible && m_Sprite.getTexture() != nullptr)
	{
		renderManager.AddDrawable(m_RenderLayer, &m_Sprite, objectTransform, m_Sprite.getRotation(), m_Sprite.getScale());
	}
}

IntVec SpriteComponent::GetSize() const
{
	if (m_Sprite.getTexture() == nullptr)
	{
		return {0,0};
	}
	else if (m_SubimagesEnabled)
	{
		return m_SubimageSize;
	}
	else
	{
		return IntVec(m_Sprite.getTexture()->getSize());
	}
}

void SpriteComponent::CentreOrigin()
{
	m_Sprite.setOrigin(0.5f*ToFVec(GetSize()));
}

void SpriteComponent::CentreOriginAndAlignToBoundingBox(Alignment alignment)
{
	CentreOrigin();
	IntVec ownerSize = GetOwner()->GetBoundsSize();
	IntVec spriteSize = GetSize();

	IntVec position;

	if (alignment == Alignment::TopLeft || alignment == Alignment::LeftCentre || alignment == Alignment::BottomLeft)
	{
		position.x = spriteSize.x/2;
	}
	else if (alignment == Alignment::TopCentre || alignment == Alignment::Centre || alignment == Alignment::BottomCentre)
	{
		position.x = ownerSize.x/2;
	}
	else
	{
		position.x = ownerSize.x - spriteSize.x/2;
	}

	if (alignment == Alignment::TopLeft || alignment == Alignment::TopCentre || alignment == Alignment::TopRight)
	{
		position.y = spriteSize.y/2;
	}
	else if (alignment == Alignment::LeftCentre || alignment == Alignment::Centre || alignment == Alignment::RightCentre)
	{
		position.y = ownerSize.y/2;
	}
	else
	{
		position.y = ownerSize.y - spriteSize.y/2;
	}

	m_Sprite.setPosition(ToFVec(position));
}

// Todo: Could support backwards animation.
void SpriteComponent::AdvanceSubimage(int framesToAdvance)
{
	m_CurrentSubimage += framesToAdvance;
	while (m_CurrentSubimage >= m_NumSubimages)
	{
		if (m_AnimationLoop)
		{
			m_CurrentSubimage -= m_NumSubimages;
		}
		else
		{
			m_CurrentSubimage = std::max(m_NumSubimages - 1, 0);
			m_AnimationRate = 0.0f;
		}
		EventAnimationEnd.Broadcast();
	}
}

void SpriteComponent::UpdateSpriteRect()
{
	int row = m_FirstSubimage.y + (m_CurrentSubimage / m_SubimagesPerRow);
	int col = m_FirstSubimage.x + (m_CurrentSubimage % m_SubimagesPerRow);

	int x = col * (m_SubimageSize.x);
	int y = row * (m_SubimageSize.y);

	m_Sprite.setTextureRect( {x, y, m_SubimageSize.x, m_SubimageSize.y} );
}
