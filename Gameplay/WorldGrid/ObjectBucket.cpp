#include "ObjectBucket.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/RenderManager.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "Util/VectorUtil.h"

void ObjectBucket::AddObject(GameObject* newObject)
{
	assert(newObject);
	assert(!Util::Contains(m_ObjectList, newObject));
	m_ObjectList.emplace_back(newObject);
	newObject->SetBucket(this);
}

void ObjectBucket::DestroyObject(GameObject* object)
{
	Util::RemoveSwapFirstMatchingItem(m_ObjectList, object);
	assert(!Util::Contains(m_ObjectList, object));
}

void ObjectBucket::TransferObject(GameObject* object, ObjectBucket& newBucket)
{
	auto itr = Util::Find(m_ObjectList, object);
	if (itr != m_ObjectList.end())
	{
		newBucket.AddObject(itr->release());
		Util::RemoveSwap(m_ObjectList, itr);
	}
}

void ObjectBucket::GatherAllObjects(ObjectRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr)
		{
			gatherList.push_back(objectPtr->GetWeakRef());
		}
	}
}

void ObjectBucket::GatherAllObjects(ObjectConstRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr)
		{
			gatherList.push_back(objectPtr->GetConstRef());
		}
	}
}

void ObjectBucket::GatherObjectsByChannel(NameHash channelName, ObjectRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr && objectPtr->GetCollisionChannel() == channelName)
		{
			gatherList.push_back(objectPtr->GetWeakRef());
		}
	}
}

void ObjectBucket::GatherObjectsByChannel(NameHash channelName, ObjectConstRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr && objectPtr->GetCollisionChannel() == channelName)
		{
			gatherList.push_back(objectPtr->GetConstRef());
		}
	}
}

void ObjectBucket::GatherObjectsHitByChannel(IntRect rect, NameHash channelName, ObjectRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr &&
			objectPtr->GetCollisionChannel() == channelName &&
			objectPtr->GetBounds().intersects(rect))
		{
			gatherList.push_back(objectPtr->GetWeakRef());
		}
	}
}

void ObjectBucket::GatherObjectsHitByChannel(IntRect rect, NameHash channelName, ObjectConstRefList& gatherList) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr &&
			objectPtr->GetCollisionChannel() == channelName &&
			objectPtr->GetBounds().intersects(rect))
		{
			gatherList.push_back(objectPtr->GetConstRef());
		}
	}
}

ObjectRef ObjectBucket::FindFirstHitByChannel(IntRect rect, NameHash channelName, GameObject* ignore) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr && objectPtr->GetCollisionChannel() == channelName)
		{
			return objectPtr->GetWeakRef();
		}
	}
	return ObjectRef();
}

void ObjectBucket::GatherDraw(RenderManager& renderManager) const
{
	for (const std::unique_ptr<GameObject>& objectPtr : m_ObjectList)
	{
		if (objectPtr)
		{
			objectPtr->GatherDraw(renderManager);
		}
	}
}
