#pragma once

#include "Core/NameHash.h"
#include "Gameplay.h"
#include "Gameplay/HitResult.h"
#include "SFML/Graphics/Rect.hpp"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	ObjectRef GetWeakRef() { return m_RefTracker.MakeReference(this); }
	ObjectConstRef GetConstRef() const { return m_RefTracker.MakeReference(this); }

	virtual void Tick(float deltaTime);

	World* GetWorld() { return m_World; }
	const World* GetWorld() const { return m_World; }

	// A persistent object is updated and drawn at all times regardless of position.
	// A non-persistent object is sorted into a world grid cell.
	// Note: Currently, this must be constant for a single object.  It cannot change over time.
	virtual bool IsPersistent() const { return false; }

	// Every object belongs to one collision channel.
	// Any channel name is valid, but there are two special values:
	//  "None" - The default.  Not be detected by other objects for collision.
	//  "Solid" - Objects that block normal movement, similar to solid tiles.
	virtual NameHash GetCollisionChannel() const { return "None"; }

	sf::IntRect GetBounds() const { return m_Bounds; }
	sf::Vector2i GetTopLeft() const { return {m_Bounds.left, m_Bounds.top}; }

	// Move one pixel at a time.  Stops if blocked by something solid.
	HitResult TryMoveX(int32 dx);
	HitResult TryMoveY(int32 dy);

	// TODO could have a float version of move that accumulates remainder.

	// Tells the world to destroy this object.  Warning: This will immediately
	// delete the object's memory and invalidate all references to the object.
	void Destroy();

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

	// For exclusive use by World::AddObject:
	void SetWorld(World* world) { m_World = world; }

	// For exclusive use by WorldGrid::AddObject and WorldGrid::UpdateObjectPosition:
	void SetCachedGridCoords(sf::Vector2i coords) { m_CachedGridCoords = coords; }
	sf::Vector2i GetCachedGridCoords() { return m_CachedGridCoords; }

	// For exclusive use by ObjectBucket::AddObject:
	void SetBucket(ObjectBucket* bucket) { m_Bucket = bucket; }

protected:
	// Pixel-perfect position and bounding box.
	sf::IntRect m_Bounds;

	// If needed, we could add the ability to define other collision phases.

private:
	World* m_World = nullptr;
	ObjectBucket* m_Bucket = nullptr;
	sf::Vector2i m_CachedGridCoords = {0,0};

	RefTracker m_RefTracker;

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};
