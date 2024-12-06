#include "Player.h"

#include <sstream>
#include "App/InputEventManager.h"
#include "App/PerfTimer.h"
#include "Gameplay/Components/BoundsDrawComponent.h"
#include "Gameplay/Components/CameraComponent.h"
#include "Gameplay/Components/ScreenFadeComponent.h"
#include "Gameplay/Components/SpriteComponent.h"
#include "Gameplay/Components/PointLightComponent.h"
#include "Gameplay/Components/TextComponent.h"
#include "Gameplay/World.h"
#include "Platformer/Components/HealthComponent.h"
#include "Platformer/Components/HealthbarComponent.h"
#include "Platformer/Systems/PlayerSystem.h"
#include "SFML/Window/Joystick.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "Util/Math.h"

//-----------------------------------------------------------------------------
// Snowed In Arcade Button Map:
// (9)
//  (3) (0) (6)
//  (2) (1) (7)
// 0 is Yellow, 1 is Red, 2 is Green, 3 is Blue, 6/7 are black, 9 is start
//-----------------------------------------------------------------------------

Player::Player(IntVec position) :
	GameObject(IntRect(position, {26, 42}))
{
}

void Player::Init()
{
	// Move player down to the ground...
	TryMoveY(2);

	m_CameraComponent = EmplaceComponent<CameraComponent>(IntVec(384,288));
	m_CameraComponent->SetOffset({c_CameraOffsetX, -c_CameraOffsetY});

	m_HealthComponent = EmplaceComponent<HealthComponent>(100.0f);
	m_HealthbarComponent = EmplaceComponent<HealthbarComponent>();

	m_ScreenFadeComponent = EmplaceComponent<ScreenFadeComponent>(sf::Color::Black);
	m_ScreenFadeComponent->StartFadeIn(2.0f, /*fromMax*/ true);

	m_SpriteComponent = EmplaceComponent<SpriteComponent>("Player");
	AnimStand();
	m_SpriteComponent->CentreOriginAndAlignToBoundingBox(SpriteComponent::Alignment::BottomCentre);
	m_SpriteComponent->EventAnimationEnd.AddDelegate(this, &Player::OnAnimationEnd);
	
	InputEventManager& inputManager = InputEventManager::Get();
	inputManager.GetKeyPressedEvent(c_JumpKey).AddWeakRef(GetWeakPlayer(), &Player::OnPressJump);
	inputManager.GetButtonPressedEvent(c_JumpButtonId).AddWeakRef(GetWeakPlayer(), &Player::OnPressJump);

	PlayerSystem* playerSystem = GetWorld()->GetSystem<PlayerSystem>();
	playerSystem->RegisterPlayer(GetWeakPlayer());

	//-------------------------------------------------------------------------
	// Debug

	//EmplaceComponent<BoundsDrawComponent>();

	m_DebugLight = EmplaceComponent<PointLightComponent>();
	m_DebugLight->SetColour({1.0f, 0.0f, 0.0f});
	m_DebugLight->SetOffset({50, 5});
	m_DebugLight->SetRadius(100.0f);
	m_DebugLight->SetEnabled(false);

	m_DebugText = EmplaceComponent<TextComponent>();
	m_DebugText->SetFont("FixedFont");
	m_DebugText->m_Text.setFillColor(sf::Color::Red);
	m_DebugText->m_Text.setString("Test");
	m_DebugText->m_Text.setCharacterSize(32);
	m_DebugText->SetVisible(false);

	inputManager.GetKeyPressedEvent(sf::Keyboard::L).AddWeakRef(GetWeakPlayer(), &Player::OnPressL);
	inputManager.GetKeyPressedEvent(sf::Keyboard::K).AddWeakRef(GetWeakPlayer(), &Player::OnPressK);

	//-------------------------------------------------------------------------
}

void Player::Tick(float deltaTime)
{
	PerfTimer timer(__FUNCTION__);

	const InputEventManager& inputManager = InputEventManager::Get();

	// Horizontal acceleration - Running or airwalking
	if (IsPressingLeft(inputManager))
	{
		if (m_OnGround)
		{
			TryRun(-1);
		}
		else
		{
			TryAirwalk(-1);
		}
	}
	else if (IsPressingRight(inputManager))
	{
		if (m_OnGround)
		{
			TryRun(1);
		}
		else
		{
			TryAirwalk(1);
		}
	}
	else
	{
		TryStop();
	}

	// Apply horizontal movement
	HitResult resultX = TryMoveX(m_SpeedX);
	if (resultX.IsHit())
	{
		m_SpeedX = 0.0f;
		if (m_AnimState == AnimState::Run)
		{
			AnimRunStop();
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
	const bool bHoldingJump = sf::Keyboard::isKeyPressed(c_JumpKey) || sf::Joystick::isButtonPressed(0,0);

	if (bPressedJump && bCanStartJump)
	{
		// Start jump
		m_OnGround = false;
		m_IsJumping = true;
		m_JumpPressedFrame = World::c_InvalidFrame; // Don't jump twice from one input.
		m_SpeedY = -c_JumpSpeed;

		AnimStandingJumpStart();
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
		m_SpeedY = std::min(m_SpeedY + c_Gravity, c_MaxFallSpeed);
		m_JumpFrames = 0;

		if (m_SpeedY > 1.0f)
		{
			AnimStandingJumpFall();
		}
	}

/*	if (m_OnGround)
	{
		m_DebugLight->SetColour(0.0f,1.0f,0.0f);
	}
	else
	{
		m_DebugLight->SetColour(1.0f,0.0f,0.0f);
	}*/
}

void Player::TryAccelerateX(int32 facing, float maxSpeed, float accel, float decel)
{
	// Abort if blocked
	IntRect testRect = GetBounds();
	testRect.left += facing;
	if (GetWorld()->CheckForSolid(testRect).IsHit())
	{
		return;
	}

	const float fFacing = static_cast<float>(facing);
	if (m_SpeedX*fFacing < 0.0f)
	{
		m_SpeedX += fFacing*decel;
	}
	else if (m_SpeedX*fFacing < maxSpeed)
	{
		// Note: We don't slow the player down if *already* faster than max speed.
		m_SpeedX = fFacing * std::min(fFacing*m_SpeedX + accel, maxSpeed);
	}
}

void Player::TryRun(int32 facing)
{
	TryAccelerateX(facing, c_RunSpeed, c_RunAccel, c_RunDecel);

	// Update sprite, camera, animation.
	const float fFacing = static_cast<float>(facing);
	if (Math::FloatGreater(m_SpeedX*fFacing, 0.0f))
	{
		m_SpriteComponent->m_Sprite.setScale(FVec(fFacing,1.0f));
		m_CameraComponent->SetTargetOffset({facing*c_CameraOffsetX, -c_CameraOffsetY});

		if (m_OnGround && m_AnimState != AnimState::Run && m_AnimState != AnimState::RunStart)
		{
			AnimRunStart();
		}
	}
}

void Player::TryAirwalk(int32 facing)
{
	TryAccelerateX(facing, c_AirwalkSpeed, c_AirwalkAccel, c_AirwalkDecel);

	// Update sprite facing and camera.
	const float fFacing = static_cast<float>(facing);
	if (Math::FloatGreater(m_SpeedX*fFacing, 0.0f))
	{
		m_SpriteComponent->m_Sprite.setScale(FVec(fFacing,1.0f));
		m_CameraComponent->SetTargetOffset({facing*c_CameraOffsetX, -c_CameraOffsetY});
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

	if (m_OnGround)
	{
		SelectAnimStand();
	}

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
		const bool bSuccess = TryCornerCorrection();
		if (bSuccess)
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
	IntRect checkRect(m_Bounds.left, m_Bounds.top + m_Bounds.height, m_Bounds.width, 1);
	return GetWorld()->CheckForSolid(checkRect).IsHit();
}

bool Player::IsPressingLeft(const InputEventManager& inputManager) const
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
		|| inputManager.GetAxis(sf::Joystick::Axis::X) < -c_JoystickDeadZone;
}

bool Player::IsPressingRight(const InputEventManager& inputManager) const
{
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Right)\
		|| inputManager.GetAxis(sf::Joystick::Axis::X) > c_JoystickDeadZone;
}

//-----------------------------------------------------------------------------
// Event Handling

void Player::OnPressJump()
{
	m_JumpPressedFrame = GetWorld()->GetTickNumber();
}

void Player::OnPressL()
{
	m_DebugLight->ToggleEnabled();
}

void Player::OnPressK()
{
	m_HealthComponent->ApplyDamage(10.0f);
}

void Player::OnPressAnyButton(uint32 buttonId)
{
	//std::stringstream s;
	//s << "Pressed " << buttonId;
	//m_DebugText->m_Text.setString(s.str());
	//m_DebugText->SetVisible(true);
}

void Player::OnAnimationEnd()
{
	switch (m_AnimState)
	{
		case AnimState::RunStart:
		{
			AnimRun();
			break;
		}
		case AnimState::RunStop:
		case AnimState::StandingJumpLand:
		{
			AnimStand();
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Animations

void Player::SelectAnimStand()
{
	if (m_AnimState == AnimState::Run || m_AnimState == AnimState::RunStart)
	{
		AnimRunStop();
	}
	else if (m_AnimState == AnimState::StandingJumpStart || m_AnimState == AnimState::StandingJumpFall)
	{
		AnimStandingJumpLand();
	}
}

void Player::AnimStand()
{
	if (m_AnimState != AnimState::Stand)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {3,1}, 1, 1, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::None);
		m_AnimState = AnimState::Stand;
	}
}

void Player::AnimRun()
{
	if (m_AnimState != AnimState::Run)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {0,0}, 6, 6, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.2f, true);
		m_AnimState = AnimState::Run;
	}
}

void Player::AnimRunStart()
{
	if (m_AnimState != AnimState::RunStart)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {4,1}, 2, 2, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.2f, true);
		m_AnimState = AnimState::RunStart;
	}
}

void Player::AnimRunStop()
{
	if (m_AnimState != AnimState::RunStop)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {0,1}, 3, 3, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.2f, false);
		m_AnimState = AnimState::RunStop;
	}
}

void Player::AnimStandingJumpStart()
{
	if (m_AnimState != AnimState::StandingJumpStart)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {0,2}, 2, 2, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.2f, false);
		m_AnimState = AnimState::StandingJumpStart;
	}
}

void Player::AnimStandingJumpFall()
{
	if (m_AnimState != AnimState::StandingJumpFall)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {2,2}, 1, 1, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::None);
		m_AnimState = AnimState::StandingJumpFall;
	}
}

void Player::AnimStandingJumpLand()
{
	if (m_AnimState != AnimState::StandingJumpLand)
	{
		m_SpriteComponent->SetupSubimages({44,44}, {4,2}, 1, 1, 0);
		m_SpriteComponent->Animate(SpriteComponent::AnimationMode::SubimagesPerTick, 0.15f, false);
		m_AnimState = AnimState::StandingJumpLand;
	}
}
