#pragma once

#include "Core/TypeInfo.h"
#include "Gameplay/Component.h"
#include "Util/Vec2.h"
#include "Util/Rect.h"

class CameraComponent : public Component
{
public:
	CameraComponent (IntVec viewSize) : m_ViewSize(viewSize) {}
	MACRO_DeclareTypeInfo(CameraComponent)

	virtual void Tick(float deltaTime) override;

	void SetTargetOffset(IntVec offset) { m_TargetOffset = offset; }
	void SetOffset(IntVec offset) { m_TargetOffset = m_Offset = offset; }
	void SetSpeed(int speed) { m_Speed = speed; }

protected:
	IntVec m_ViewSize;
	IntVec m_Offset = {0, 0};
	IntVec m_TargetOffset = {0, 0};
	int m_Speed = 5;
};
