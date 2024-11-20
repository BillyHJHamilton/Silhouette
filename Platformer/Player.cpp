#include "Player.h"

#include "App/GameApp.h"
#include "App/PerfTimer.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/Components/SpriteComponent.h"
#include "Gameplay/World.h"
#include "SFML/Window/Keyboard.hpp"
#include "Util/Math.h"

Player::Player(IntVec position) :
	GameObject(IntRect(position, {15, 32}))
{
}

void Player::Init()
{
	m_CameraComponent = EmplaceComponent<CameraComponent>(IntVec(320,240));
	m_CameraComponent->SetSpeed(2);
	m_CameraComponent->SetOffset({c_CameraOffsetX, -c_CameraOffsetY});

	m_SpriteComponent = EmplaceComponent<SpriteComponent>("Player");
	m_SpriteComponent->SetupSubimages({32,32}, {0,0}, 1, 4, 0);
	m_SpriteComponent->m_Sprite.setPosition(ToFVec(8, 16));
	m_SpriteComponent->m_Sprite.setOrigin(ToFVec(16, 16));

	// Bounding box is smaller than sprite, so move over to centre us in the tile.
	TryMoveX(8);

	GameApp::GetInputEventManager().GetKeyPressedEvent(sf::Keyboard::LControl).AddWeakRef(GetWeakPlayer(), &Player::OnPressJump);
}

void Player::Tick(float deltaTime)
{
	PerfTimer timer(__FUNCTION__);

	// Horizontal movement.
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		TryRun(-1.0f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		TryRun(1.0f);
	}
	else
	{
		TryStop();
	}

	TryMoveX(m_SpeedX);

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
void Player::TryRun(float facing)
{
	// Add speed
	if (m_SpeedX*facing < 0.0f)
	{
		m_SpeedX += facing*c_RunDecel;
	}
	else if (m_SpeedX*facing < c_RunSpeed)
	{
		m_SpeedX += facing*c_RunAccel;
	}

	// Prevent overshooting
	if (m_SpeedX*facing > c_RunSpeed)
	{
		m_SpeedX = c_RunSpeed*facing;
	}

	// Update sprite, camera, animation.
	if (m_SpeedX*facing > 0.0f)
	{
		int32 intFacing = static_cast<int>(facing * (1.0f + Math::c_FloatEpsilon));
		m_SpriteComponent->m_Sprite.setScale(FVec(facing,1.0f));
		//m_CameraComponent->SetTargetOffset({intFacing*c_CameraOffsetX, -c_CameraOffsetY});
		AnimRun();
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

	if (std::abs(m_SpeedX) < Math::c_FloatEpsilon)
	{
		m_SpeedX = 0.0f;
		AnimStand();
	}
}

void Player::OnPressJump()
{
	m_JumpPressedFrame = GetWorld()->GetTickNumber();
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

//-----------------------------------------------------------------------------
// Animations

void Player::AnimStand()
{
	if (m_AnimState != AnimState::Stand)
	{
		m_SpriteComponent->SetupSubimages({32,32}, {0,0}, 1, 1, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::None);
		m_AnimState = AnimState::Stand;
	}
}

void Player::AnimRun()
{
	if (m_AnimState != AnimState::Run)
	{
		m_SpriteComponent->SetupSubimages({32,32}, {1,0}, 3, 3, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.15f, true);
		m_AnimState = AnimState::Run;
	}
}
