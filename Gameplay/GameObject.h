#pragma once

#include "Core/NameHash.h"
#include "Gameplay.h"
#include "Gameplay/HitResult.h"
#include "Util/Rect.h"
#include "Util/Vec2.h"

class GameObject
{
public:
	GameObject(IntRect bounds);
	virtual ~GameObject();

	ObjectRef GetWeakRef() { return m_RefTracker.MakeReference(this); }
	ObjectConstRef GetConstRef() const { return m_RefTracker.MakeReference(this); }

	// Called after the object has been added to the world.
	virtual void Init() {};

	// Called on a framely basis while within the simulation area.
	void GameObjectTick(float deltaTime, bool isPaused);
	virtual void Tick(float deltaTime) {}

	// Whether to run the derived object Tick when the game is paused.
	// Components ignore this and tick depending on their own ShouldTickWhenPaused function.
	virtual bool ShouldTickWhenPaused() const { return false; }

	virtual void GatherDraw(RenderManager& renderManager) const;

	World* GetWorld() { return m_World; }
	const World* GetWorld() const { return m_World; }

	Component* AddComponent(Component* newComponent);
	Component* FindComponentByType(NameHash typeName);
	const Component* FindComponentByType(NameHash typeName) const;

	template<typename ComponentType> ComponentType* AddComponent();
	template<typename ComponentType, typename... ArgumentList> ComponentType* EmplaceComponent(ArgumentList... args);

	template<typename ComponentType> ComponentType* FindComponent();
	template<typename ComponentType> const ComponentType* FindComponent() const;

	template<typename ComponentType> std::vector<ComponentType*> FindAllComponentsByType();
	template<typename ComponentType> std::vector<const ComponentType*> FindAllComponentsByType() const;

	// A persistent object is updated and drawn at all times regardless of position.
	// A non-persistent object is sorted into a world grid cell.
	// Note: Currently, this must be constant for a single object.  It cannot change over time.
	virtual bool IsPersistent() const { return false; }

	// Every object belongs to one collision channel.
	// Any channel name is valid, but there are two special values:
	//  "None" - The default.  Not be detected by other objects for collision.
	//  "Solid" - Objects that block normal movement, similar to solid tiles.
	virtual NameHash GetCollisionChannel() const { return "None"; }

	IntRect GetBounds() const { return m_Bounds; }
	IntVec GetTopLeft() const { return {m_Bounds.left, m_Bounds.top}; }
	IntVec GetCentre() const { return {m_Bounds.left + m_Bounds.width/2, m_Bounds.top + m_Bounds.height/2}; }
	IntVec GetBoundsSize() const { return {m_Bounds.width, m_Bounds.height}; }

	// Move one pixel at a time.  Stops if blocked by something solid.
	HitResult TryMoveX(int32 dx);
	HitResult TryMoveY(int32 dy);

	// As above, but accumulates fractional movement until a full pixel can be moved.
	HitResult TryMoveX(float dx);
	HitResult TryMoveY(float dy);

	// Tells the world to destroy this object.  Warning: This will immediately
	// delete the object's memory and invalidate all references to the object.
	void Destroy();

	// For exclusive use by World::AddObject:
	void SetWorld(World* world) { m_World = world; }

	// For exclusive use by WorldGrid::AddObject and WorldGrid::UpdateObjectPosition:
	void SetCachedGridCoords(IntVec coords) { m_CachedGridCoords = coords; }
	IntVec GetCachedGridCoords() { return m_CachedGridCoords; }

	// For exclusive use by ObjectBucket::AddObject:
	void SetBucket(ObjectBucket* bucket) { m_Bucket = bucket; }

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

protected:
	RefTracker m_RefTracker;

	// Pixel-perfect position and bounding box.
	IntRect m_Bounds;

	// If needed, we could add the ability to define other collision phases.

	float m_MoveRemainderX = 0;
	float m_MoveRemainderY = 0;

	std::vector<std::unique_ptr<Component>> m_ComponentList;

private:
	World* m_World = nullptr;
	ObjectBucket* m_Bucket = nullptr;
	IntVec m_CachedGridCoords = {0,0};

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};

// Template implementations

template<typename ComponentType>
ComponentType* GameObject::AddComponent()
{
	return static_cast<ComponentType*>(AddComponent(new ComponentType));
}

template<typename ComponentType, typename... ArgumentList>
ComponentType* GameObject::EmplaceComponent(ArgumentList... args)
{
	return static_cast<ComponentType*>(AddComponent(new ComponentType(std::forward<ArgumentList>(args)...)));
}

template<typename ComponentType>
ComponentType* GameObject::FindComponent()
{
	return static_cast<ComponentType*>(FindComponentByType(ComponentType::StaticType()));
}

template<typename ComponentType>
const ComponentType* GameObject::FindComponent() const
{
	return static_cast<const ComponentType*>(FindComponentByType(ComponentType::StaticType()));
}

template<typename ComponentType>
std::vector<ComponentType*> GameObject::FindAllComponentsByType()
{
	std::vector<ComponentType*> output;
	output.reserve(5); // should be enough I guess
	for (const std::unique_ptr<Component>& NextComponent : m_ComponentList)
	{
		if (NextComponent->GetTypeName() == ComponentType::StaticType())
		{
			output.push_back(static_cast<ComponentType*>(NextComponent.get()));
		}
	}
	return output;
}

template<typename ComponentType>
std::vector<const ComponentType*> GameObject::FindAllComponentsByType() const
{
	std::vector<const ComponentType*> output;
	output.reserve(5); // should be enough I guess
	for (const std::unique_ptr<Component>& NextComponent : m_ComponentList)
	{
		if (NextComponent->GetTypeName() == ComponentType::StaticType())
		{
			output.push_back(static_cast<ComponentType*>(NextComponent.get()));
		}
	}
	return output;
}