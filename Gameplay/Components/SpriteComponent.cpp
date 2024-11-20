#include "SpriteComponent.h"

#include "App/GameApp.h"
#include "App/ShaderManager.h"
#include "Core/TypeInfo.h"
#include "Gameplay/GameObject.h"
#include "SFML/Graphics/RenderTarget.hpp"

#include "SFML/Graphics/Shader.hpp"
#include "SFML/Graphics/Transform.hpp"
#include "Util/Math.h"

TypeInfo const SpriteComponent::s_TypeInfo = TypeInfo("SpriteComponent", {"Component"});

SpriteComponent::SpriteComponent(NameHash textureName)
{
	const sf::Texture* spriteTexture = GameApp::GetAssetManager().FindTexture(textureName);
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

void SpriteComponent::Draw(sf::RenderTarget& RenderTarget, const sf::RenderStates& renderStates) const
{
	if (m_Visible && m_Sprite.getTexture() != nullptr)
	{
		ShaderManager& shaderManager = GameApp::GetShaderManager();
		shaderManager.SetNormalTransform(m_Sprite.getRotation(), m_Sprite.getScale());
		RenderTarget.draw(m_Sprite, renderStates);
		shaderManager.ClearNormalTransform();
	}
}

// Todo: Could support backwards animation.
void SpriteComponent::AdvanceSubimage(int framesToAdvance)
{
	m_CurrentSubimage += framesToAdvance;
	while (m_CurrentSubimage >= m_NumSubimages)
	{
		EventAnimationEnd.Broadcast();
		m_CurrentSubimage -= m_NumSubimages;
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
