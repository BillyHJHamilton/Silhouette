#pragma once

#include "Core/TypeInfo.h"
#include "Gameplay/Gameplay.h"
#include "SFML/Graphics/Transform.hpp"

class Component : public ITypeInfoProvider
{
public:
	Component();
	virtual ~Component();
	MACRO_DeclareTypeInfo(Component)

	virtual void OnAddedToObject(GameObject& newOwner) {};

	const GameObject* GetOwner() const { return m_Owner; }
	GameObject* GetOwner() { return m_Owner; }

	const World* GetWorld() const;
	World* GetWorld();

	virtual bool ShouldTickWhenPaused() const { return false; }

	virtual void Tick(float deltaTime) {};
	virtual void GatherDraw(RenderManager& renderManager, const sf::Transform& ObjectTransform) const {}

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

private:
	friend class GameObject;
	void SetOwner(GameObject* newOwner);

	GameObject* m_Owner = nullptr;

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};
