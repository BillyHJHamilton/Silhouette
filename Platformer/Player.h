#pragma once

#include "Gameplay/GameObject.h"

class CameraComponent;
class SpriteComponent;

class Player : public GameObject
{
public:
	Player(IntVec position);
	WeakRef<Player> GetWeakPlayer() { return m_RefTracker.MakeReference(this); }

	static constexpr int32 c_CameraOffsetX = 32;
	static constexpr int32 c_CameraOffsetY = 32;

	static constexpr float c_RunSpeed = 6.0f;
	static constexpr float c_RunAccel = 0.4f;
	static constexpr float c_RunDecel = 0.4f;

	static constexpr float c_JumpSpeed = 5.5f;
	static constexpr float c_JumpGravity = 0.25f;
	static constexpr float c_Gravity = 0.5f;
	static constexpr int32 c_MaxJumpFrames = 10;

	static constexpr int32 c_MaxCornerCorrection = 3;
	static constexpr int32 c_MaxJumpDelay = 5;

	virtual void Init() override;

	virtual void Tick(float deltaTime) override;

	virtual bool IsPersistent() const { return true; }

protected:
	enum class AnimState
	{
		None,
		Stand,
		Run,
		StartRun
	};
	AnimState m_AnimState = AnimState::None;

	void TryRun(int32 facing);
	void TryStop();
	void OnBlockedY(HitResult& hitResult);
	bool TryCornerCorrection();

	bool CheckOnGround() const;

	void OnPressJump();
	void OnAnimationEnd();

	void AnimStand();
	void AnimRun();
	void AnimStartRun();

	CameraComponent* m_CameraComponent = nullptr;
	SpriteComponent* m_SpriteComponent = nullptr;

	float m_SpeedX = 0.0f;
	float m_SpeedY = 0.0f;
	bool m_OnGround = true;

	bool m_IsJumping = false;
	int32 m_JumpFrames = c_MaxJumpFrames;

	int32 m_JumpPressedFrame = -c_MaxJumpDelay - 1;
};
