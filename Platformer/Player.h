#pragma once

#include "Gameplay/GameObject.h"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Joystick.hpp"

class CameraComponent;
class PointLightComponent;
class SpriteComponent;
class TextComponent;

class Player : public GameObject
{
public:
	Player(IntVec position);
	WeakRef<Player> GetWeakPlayer() { return m_RefTracker.MakeReference(this); }

	static constexpr sf::Keyboard::Key c_JumpKey = sf::Keyboard::Key::LControl;
	static constexpr uint32 c_JumpButtonId = 3;

	static constexpr int32 c_CameraOffsetX = 0;
	static constexpr int32 c_CameraOffsetY = 22;

	static constexpr float c_RunSpeed = 6.0f;
	static constexpr float c_RunAccel = 0.4f;
	static constexpr float c_RunDecel = 0.4f;

	static constexpr float c_JumpSpeed = 7.0f;
	static constexpr float c_JumpGravity = 0.15f;
	static constexpr float c_Gravity = 0.4f;
	static constexpr int32 c_MaxJumpFrames = 14;

	static constexpr int32 c_MaxCornerCorrection = 6;
	static constexpr int32 c_MaxJumpDelay = 5;

	static constexpr float c_JoystickDeadZone = 10.0f; // Out of 100

	virtual void Init() override;

	virtual void Tick(float deltaTime) override;

	virtual bool IsPersistent() const { return true; }

protected:
	enum class AnimState
	{
		None,
		Stand,
		Run,
		RunStart,
		StandingJumpStart,
		StandingJumpFall
	};
	AnimState m_AnimState = AnimState::None;

	void TryRun(int32 facing);
	void TryStop();
	void OnBlockedY(HitResult& hitResult);
	bool TryCornerCorrection();

	bool CheckOnGround() const;

	void OnPressJump();
	void OnPressL();
	void OnPressAnyButton(uint32 buttonId);
	void OnAnimationEnd();

	void AnimStand();
	void AnimRun();
	void AnimRunStart();
	void AnimStandingJumpStart();
	void AnimStandingJumpFall();

	CameraComponent* m_CameraComponent = nullptr;
	SpriteComponent* m_SpriteComponent = nullptr;

	PointLightComponent* m_DebugLight = nullptr;
	TextComponent* m_DebugText = nullptr;

	float m_SpeedX = 0.0f;
	float m_SpeedY = 0.0f;
	bool m_OnGround = true;

	bool m_IsJumping = false;
	int32 m_JumpFrames = c_MaxJumpFrames;

	int32 m_JumpPressedFrame = -c_MaxJumpDelay - 1;
};
