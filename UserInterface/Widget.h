#pragma once

#include "Core/Reference.h"

class RenderManager;
class World;

class Widget
{
public:
	virtual void Tick(float deltaTime) {}
	virtual void GatherDraw(RenderManager& renderManager) const;

	WeakRef<Widget> GetWeakRef() { return m_RefTracker.MakeReference(this); }

	World* GetWorld() { return m_World; }

	// For exclusive use by World::AddWidget:
	void SetWorld(World* world) { m_World = world; }

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

protected:
	RefTracker m_RefTracker;

private:
	World* m_World = nullptr;

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};
