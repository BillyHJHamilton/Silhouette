#pragma once

#include "Core/Reference.h"

class RenderManager;
class World;

class Widget
{
public:
	virtual void Tick(float deltaTime) {}
	virtual void GatherDraw(RenderManager& renderManager) const {};

	WeakRef<Widget> GetWeakRef() { return m_RefTracker.MakeReference(this); }

	World* GetWorld() { return m_World; }

	int32 GetDepth() { return m_Depth; }
	void SetDepth(int32 newDepth) { m_Depth = newDepth; }

	// For exclusive use by World::AddWidget:
	void SetWorld(World* world) { m_World = world; }

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

protected:
	RefTracker m_RefTracker;

	// Higher number means draw first.
	int32 m_Depth = 0;

private:
	World* m_World = nullptr;

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};
