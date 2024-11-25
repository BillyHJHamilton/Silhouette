#pragma once

#include "Core/Event.h"
#include "Core/TypeInfo.h"
#include "Gameplay/Component.h"
#include "Gameplay/RenderManager.h"
#include "SFML/Graphics/Sprite.hpp"
#include "Util/Vec2.h"
#include "Util/Rect.h"

class SpriteComponent : public Component
{
public:
	SpriteComponent(NameHash textureName);
	MACRO_DeclareTypeInfo(SpriteComponent)

	void SetVisible(bool visibility) { m_Visible = visibility; };
	void SetLayer(RenderLayer::Enum layer) { m_RenderLayer = layer; }

	enum class AnimationMode
	{
		None,
		SubimagesPerTick,
		SubimagesPerSecond
	};

	// The subimage system lets us select part of a larger spritesheet texture.
	// It also allows for flipbook animation, if m_NumSubimages is greater than 1.
	// The texture must have a uniform grid layout with no spacing between subimages.
	// The images to use in a single animation must be laid out in a rectangular section.
	// The section to use is set by firstSubimage, numSubimages, and subimagesPerRow.
	void SetupSubimages(IntVec subimageSize, IntVec firstSubimage, int numSubimages, int subimagesPerRow, int currentSubimage);
	void Animate(AnimationMode mode = AnimationMode::SubimagesPerTick, float rate = 1.0f, bool loop = true);
	MulticastEvent<> EventAnimationEnd;

	virtual void Tick(float deltaTime) override;
	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& objectTransform) const override;

	// Make sure to set up subimages before calling this, or it will return the full texture size.
	IntVec GetSize() const;

	// The following functions depend on GetSize, so set up subimages before using:
	void CentreOrigin();

	enum class Alignment { TopLeft, TopCentre, TopRight, LeftCentre, Centre, RightCentre, BottomLeft, BottomCentre, BottomRight };
	void CentreOriginAndAlignToBoundingBox(Alignment alignment);

	// Public to allow changing local transform, etc.
	sf::Sprite m_Sprite;

private:
	void AdvanceSubimage(int framesToAdvance);
	void UpdateSpriteRect();

	bool m_Visible = true;
	RenderLayer::Enum m_RenderLayer = RenderLayer::MainObjects_Lit;

	bool m_SubimagesEnabled = false;
	IntVec m_SubimageSize = {32, 32};
	IntVec m_FirstSubimage = {0, 0}; // Position (in tiles) of first subimage.
	int m_NumSubimages = 1;
	int m_SubimagesPerRow = 1;
	int m_CurrentSubimage = 0; // Always goes from 0 to m_NumSubimages - 1, regardless of m_FirstSubimage.

	AnimationMode m_AnimationMode = AnimationMode::None;
	bool m_AnimationLoop = true;
	float m_AnimationRate = 1.0f;
	float m_AnimationCounter = 0.0f;
};
