#pragma once

#include "Core/TypeInfo.h"
#include "Gameplay/Component.h"
#include "SFML/Graphics/View.hpp"
#include "Util/Vec2.h"
#include "Util/Rect.h"

class CameraComponent : public Component
{
public:
	CameraComponent (IntVec viewSize) : m_ViewSize(viewSize) {}
	MACRO_DeclareTypeInfo(CameraComponent)

	virtual void Tick(float deltaTime) override;

	// Instantly set the view to a position, relative to the owner object.
	// Optionally also set this as the target.
	void SetOffset(IntVec offset, bool updateTarget = true);

	// Sets the position the view will move towards, relative to the owner object.
	void SetTargetOffset(IntVec offset) { m_TargetOffset = offset; }

	// Set how fast the camera accelerates towards the target offset.
	void SetSpeed(float accel, float maxSpeed);

protected:
	static float FindStoppingDistance(float speed, float decel);
	static float FindNewSpeed(float dist, float speed, float accel, float maxSpeed);

	IntVec m_Position = {0, 0};
	IntVec m_ViewSize;
	IntVec m_TargetOffset = {0, 0};
	float m_MaxSpeed = 8.0f;
	float m_Accel = 0.8f;
	FVec m_Velocity = {0.0f, 0.0f};
	FVec m_MoveRemainder = {0.0f, 0.0f};
};
