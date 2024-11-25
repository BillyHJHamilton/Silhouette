#pragma once

#include <unordered_map>
#include <vector>
#include "Core/NameHash.h"
#include "Gameplay/Gameplay.h"
#include "SFML/Graphics/Drawable.hpp"
#include "Util/Rect.h"

class GameObject;

// Contains game objects and controls their lifetime.
// Provides methods to easily gather WeakRefs to objects in the bucket.

class ObjectBucket
{
public:
	void AddObject(GameObject* newObject);
	void DestroyObject(GameObject* object);

	// If a matching object is found in this bucket, it is transferred to newBucket.
	void TransferObject(GameObject* object, ObjectBucket& newBucket);

	// When we gather, we provide a list of WeakRefs to objects in this bucket.
	// This can be used to inspect, modify, or destroy objects as required.
	// There are const ref variants if you only need to inspect or draw objects.  (Though maybe we don't need this)
	void GatherAllObjects(ObjectRefList& gatherList) const;
	void GatherAllObjects(ObjectConstRefList& gatherList) const;
	void GatherObjectsByChannel(NameHash channelName, ObjectRefList& gatherList) const;
	void GatherObjectsByChannel(NameHash channelName, ObjectConstRefList& gatherList) const;
	void GatherObjectsHitByChannel(IntRect rect, NameHash channelName, ObjectRefList& gatherList) const;
	void GatherObjectsHitByChannel(IntRect rect, NameHash channelName, ObjectConstRefList& gatherList) const;

	ObjectRef FindFirstHitByChannel(IntRect rect, NameHash channelName, GameObject* ignore = nullptr) const;

	void GatherDraw(RenderManager& renderManager) const;

protected:
	std::vector<std::unique_ptr<GameObject>> m_ObjectList;
};
