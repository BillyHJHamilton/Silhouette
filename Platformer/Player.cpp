#include "Player.h"

#include "App/GameApp.h"
#include "App/PerfTimer.h"
#include "Gameplay/Components/BoundsDrawComponent.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/Components/SpriteComponent.h"
#include "Gameplay/Components/PointLightComponent.h"
#include "Gameplay/World.h"
#include "SFML/Window/Keyboard.hpp"
#include "Util/Math.h"

// Debug

Player::Player(IntVec position) :
	GameObject(IntRect(position, {26, 42}))
{
}

void Player::Init()
{
	// Debug
	//EmplaceComponent<BoundsDrawComponent>();

	//m_DebugLight = EmplaceComponent<PointLightComponent>();
	//m_DebugLight->SetColour({1.0f, 0.0f, 0.0f});
	//m_DebugLight->SetOffset({50, 5});
	//m_DebugLight->SetRadius(100.0f);

	m_CameraComponent = EmplaceComponent<CameraComponent>(IntVec(320,240));
	m_CameraComponent->SetSpeed(2);
	m_CameraComponent->SetOffset({c_CameraOffsetX, -c_CameraOffsetY});

	m_SpriteComponent = EmplaceComponent<SpriteComponent>("Player");
	AnimStand();
	m_SpriteComponent->CentreOriginAndAlignToBoundingBox(SpriteComponent::Alignment::BottomCentre);
	m_SpriteComponent->EventAnimationEnd.AddDelegate(this, &Player::OnAnimationEnd);

	// Tile grid doesn't match right now... Move player up
	m_Bounds.top -= 10;

	// Bounding box is smaller than sprite, so move over to centre us in the tile.
	TryMoveX(9);
	TryMoveY(2);

	GameApp::GetInputEventManager().GetKeyPressedEvent(sf::Keyboard::LControl).AddWeakRef(GetWeakPlayer(), &Player::OnPressJump);
}

void Player::Tick(float deltaTime)
{
	PerfTimer timer(__FUNCTION__);

	// Running
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		TryRun(-1);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		TryRun(1);
	}
	else
	{
		TryStop();
	}

	// Horizontal movement
	HitResult resultX = TryMoveX(m_SpeedX);
	if (resultX.IsHit())
	{
		m_SpeedX = 0.0f;
		if (m_AnimState == AnimState::Run)
		{
			AnimStand();
		}
	}

	// Vertical movement
	HitResult resultY = TryMoveY(m_SpeedY);
	if (resultY.IsHit())
	{
		OnBlockedY(resultY);
	}

	m_OnGround = (m_SpeedY == 0) && CheckOnGround();

	if (m_OnGround)
	{
		m_JumpFrames = c_MaxJumpFrames;
	}

	// Jumping and gravity
	const bool bPressedJump = GetWorld()->GetTicksSince(m_JumpPressedFrame) < c_MaxJumpDelay;
	const bool bCanStartJump = m_OnGround && !m_IsJumping;
	const bool bHoldingJump = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl);

	if (bPressedJump && bCanStartJump)
	{
		// Start jump
		m_OnGround = false;
		m_IsJumping = true;
		m_JumpPressedFrame = World::c_InvalidFrame; // Don't jump twice from one input.
		m_SpeedY = -c_JumpSpeed;
	}
	else if (m_IsJumping && bHoldingJump && m_JumpFrames > 0)
	{
		// Continue jump - Reduced gravity if you hold down the button.
		m_SpeedY += c_JumpGravity;
		--m_JumpFrames;
	}
	else if (!m_OnGround)
	{
		// Normal gravity
		m_SpeedY += c_JumpGravity;
		m_JumpFrames = 0;
	}
}

void Player::TryRun(int32 facing)
{
	// Abort if blocked
	IntRect testRect = GetBounds();
	testRect.left += facing;
	if (GetWorld()->CheckForSolid(testRect).IsHit())
	{
		return;
	}

	const float fFacing = static_cast<float>(facing);

	// Add speed
	if (m_SpeedX*fFacing < 0.0f)
	{
		m_SpeedX += fFacing*c_RunDecel;
	}
	else if (m_SpeedX*fFacing < c_RunSpeed)
	{
		m_SpeedX += fFacing*c_RunAccel;
	}

	// Prevent overshooting
	if (m_SpeedX*fFacing > c_RunSpeed)
	{
		m_SpeedX = c_RunSpeed*fFacing;
	}

	// Update sprite, camera, animation.
	if (m_SpeedX*fFacing > 0.0f)
	{
		m_SpriteComponent->m_Sprite.setScale(FVec(fFacing,1.0f));
		//m_CameraComponent->SetTargetOffset({facing*c_CameraOffsetX, -c_CameraOffsetY});

		if (m_AnimState != AnimState::Run && m_AnimState != AnimState::StartRun)
		{
			AnimStartRun();
			//AnimRun();
		}
	}
}

void Player::TryStop()
{
	const float oldSpeed = std::abs(m_SpeedX);
	if (oldSpeed > Math::c_FloatEpsilon)
	{
		const float sign = Math::Sign(m_SpeedX);
		m_SpeedX = sign * std::max(0.0f, (oldSpeed - c_RunDecel));
	}

	AnimStand();
	if (std::abs(m_SpeedX) < Math::c_FloatEpsilon)
	{
		m_SpeedX = 0.0f;
	}
}

void Player::OnBlockedY(HitResult& hitResult)
{
	const bool bGoingUp = m_SpeedY < 0.0f;

	// Corner correction
	if (bGoingUp)
	{
		if (TryCornerCorrection())
		{
			return;
		}
		
		m_JumpFrames = 0;
	}

	m_SpeedY = 0.0f;
	m_IsJumping = false;
}

bool Player::TryCornerCorrection()
{
	IntRect testRect = m_Bounds;
	testRect.top += Math::IntSign(m_SpeedY);
	for (int32 x = 1; x <= c_MaxCornerCorrection; ++x)
	{
		testRect.left = m_Bounds.left + x;
		if (!GetWorld()->CheckForSolid(testRect).IsHit())
		{
			TryMoveX(x);
			return true;
		}
	}
	for (int32 x = -1; x >= -c_MaxCornerCorrection; --x)
	{
		testRect.left = m_Bounds.left + x;
		if (!GetWorld()->CheckForSolid(testRect).IsHit())
		{
			TryMoveX(x);
			return true;
		}
	}

	return false;
}

bool Player::CheckOnGround() const
{
	IntRect checkRect(m_Bounds.left + 2, m_Bounds.top + m_Bounds.height, m_Bounds.width - 4, 1);
	return GetWorld()->CheckForSolid(checkRect).IsHit();
}

void Player::OnPressJump()
{
	m_JumpPressedFrame = GetWorld()->GetTickNumber();
}

void Player::OnAnimationEnd()
{
	if (m_AnimState == AnimState::StartRun)
	{
		AnimRun();
	}
}

//-----------------------------------------------------------------------------
// Animations

void Player::AnimStand()
{
	if (m_AnimState != AnimState::Stand)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {0,1}, 4, 4, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerSecond, 12.0f, false);
		m_AnimState = AnimState::Stand;
	}
}

void Player::AnimRun()
{
	if (m_AnimState != AnimState::Run)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {0,0}, 6, 6, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerSecond, 12.0f, true);
		m_AnimState = AnimState::Run;
	}
}

void Player::AnimStartRun()
{
	if (m_AnimState != AnimState::StartRun)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {4,1}, 2, 2, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerSecond, 12.0f, true);
		m_AnimState = AnimState::StartRun;
	}
}
