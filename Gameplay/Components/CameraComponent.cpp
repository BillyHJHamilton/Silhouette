#include "CameraComponent.h"

#include <algorithm>
#include "Gameplay/GameObject.h"
#include "Gameplay/World.h"
#include "SFML/Graphics/View.hpp"
#include "Util/Math.h"

TypeInfo const CameraComponent::s_TypeInfo = TypeInfo("CameraComponent", {"Component"});

void CameraComponent::Tick(float deltaTime)
{
	IntVec targetPos = GetOwner()->GetCentre() + m_TargetOffset;

	IntVec posDiff = targetPos - m_Position;


	// Apply acceleration

	const float signX = Math::FloatSign(posDiff.x);
	const float diffX = posDiff.x * signX;
	const float vX = m_Velocity.x * signX;
	if (diffX > 0.0f || !Math::FloatIsZero(vX))
	{
		m_Velocity.x = signX * FindNewSpeed(diffX, vX, m_Accel, m_MaxSpeed);
	}

	const float signY = Math::FloatSign(posDiff.y);
	const float diffY = posDiff.y * signY;
	const float vY = m_Velocity.y * signY;
	if (diffY > 0.0f || !Math::FloatIsZero(vY))
	{
		m_Velocity.y = signY * FindNewSpeed(diffY, vY, m_Accel, m_MaxSpeed);
	}

	// Apply velocity
	m_MoveRemainder += m_Velocity;
	IntVec intMove = TruncateToIntVec(m_MoveRemainder);
	m_MoveRemainder -= ToFVec(intMove);
	m_Position += intMove;

	// Stop if on target.
	if (m_Position.x == targetPos.x && Math::FloatNearlyZero(m_Velocity.x, 1.0f))
	{
		m_MoveRemainder.x = 0;
		m_Velocity.x = 0.0f;
	}
	if (m_Position.y == targetPos.y && Math::FloatNearlyZero(m_Velocity.y, 1.0f))
	{
		m_MoveRemainder.y = 0;
		m_Velocity.y = 0.0f;
	}

	// Update view
	sf::View view(ToFVec(m_Position), ToFVec(m_ViewSize));
	GetWorld()->SetMainView(view);
}

void CameraComponent::SetOffset(IntVec offset, bool updateTarget)
{
	m_Position = GetOwner()->GetCentre() + offset;
	if (updateTarget)
	{
		m_TargetOffset = offset;
	}
}

void CameraComponent::SetSpeed(float accel, float maxSpeed)
{
	m_Accel = accel;
	m_MaxSpeed = maxSpeed;
}

// Note: I don't think this is exactly right, but it should be close enough.
/*static*/ float CameraComponent::FindStoppingDistance(float speed, float decel)
{
	assert(decel != 0.0f);
	return speed * (1 + speed/decel) * 0.5f;
}

/*static*/ float CameraComponent::FindNewSpeed(float dist, float speed, float accel, float maxSpeed)
{
	const float stoppingDistance = FindStoppingDistance(speed, accel);
	if (speed <= 0.0f)
	{
		// Going wrong direction.  Accelerate towards target.
		return std::min(speed + accel, maxSpeed);
	}
	else if (dist > 1.5f * stoppingDistance)
	{
		// Going right direction and can afford to accelerate.
		return std::min(speed + accel, maxSpeed);
	}
	else if (dist <= stoppingDistance)
	{
		// We're getting close, slow down.
		return std::max(accel, speed - accel);
	}
	else
	{
		// Hold the course.
		return speed;
	}
}
