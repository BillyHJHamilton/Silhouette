#include "GameObject.h"

#include "Gameplay/Component.h"
#include "Gameplay/WorldGrid/ObjectBucket.h"
#include "Gameplay/World.h"
#include "Util/Math.h"

GameObject::GameObject(IntRect bounds) :
	m_Bounds(bounds)
{
	#if DEBUG_MEMORY
		++s_NumCreated;
	#endif
}

GameObject::~GameObject()
{
	#if DEBUG_MEMORY
		++s_NumDestroyed;
	#endif
}

#if DEBUG_MEMORY
int GameObject::s_NumCreated = 0;
int GameObject::s_NumDestroyed = 0;

void GameObject::CheckMemoryReleased()
{
	if (s_NumCreated == s_NumDestroyed)
	{
		std::cout << "Object Memory OK." << std::endl;
	}
	else
	{
		std::cerr << "Object Memory Error!  Created " << s_NumCreated
			<< ", Destroyed " << s_NumDestroyed << std::endl;
	}
}
#endif

void GameObject::GameObjectTick(float deltaTime)
{
	assert(m_World != nullptr);
	assert(m_Bucket != nullptr);

	// Tick components
	for (std::unique_ptr<Component>& nextComponent : m_ComponentList)
	{
		nextComponent->Tick(deltaTime);
	}

	// Trigger derived class trick.
	Tick(deltaTime);
}

Component* GameObject::AddComponent(Component* newComponent)
{
	assert(newComponent != nullptr);
	m_ComponentList.emplace_back(newComponent);
	newComponent->SetOwner(this);
	return newComponent;
}

const Component* GameObject::FindComponentByType(NameHash typeName) const
{
	for (const std::unique_ptr<Component>& NextComponent : m_ComponentList)
	{
		if (NextComponent->GetTypeName() == typeName)
		{
			return NextComponent.get();
		}
	}
	return nullptr;
}

Component* GameObject::FindComponentByType(NameHash typeName)
{
	return const_cast<Component*>(const_cast<const GameObject*>(this)->FindComponentByType(typeName));
}

// Move code inspired by "Celeste & TowerFall Physics"
// https://maddymakesgames.com/articles/celeste_and_towerfall_physics/index.html

HitResult GameObject::TryMoveX(int32 dx)
{
	HitResult result; // NoHit by default
	bool moved = false;
	
	int32 sign = Math::Sign(dx);
	while (dx != 0)
	{
		IntRect testRect = m_Bounds;
		testRect.left += sign;

		result = m_World->CheckForSolid(testRect, this);
		if (result.IsHit())
		{
			break;
		}
		else
		{
			m_Bounds.left += sign;
			dx -= sign;
			moved = true;
		}
	}

	if (moved && !IsPersistent())
	{
		m_World->UpdateObjectPosition(this);
	}

	return result;
}

HitResult GameObject::TryMoveY(int32 dy)
{
	HitResult result; // NoHit by default
	bool moved = false;

	int32 sign = Math::Sign(dy);
	while (dy != 0)
	{
		IntRect testRect = m_Bounds;
		testRect.top += sign;

		result = m_World->CheckForSolid(testRect, this);
		if (result.IsHit())
		{
			break;
		}
		else
		{
			m_Bounds.top += sign;
			dy -= sign;
			moved = true;
		}
	}

	if (moved && !IsPersistent())
	{
		m_World->UpdateObjectPosition(this);
	}

	return result;
}

HitResult GameObject::TryMoveX(float dx)
{
	m_MoveRemainderX += dx;
	int32 intMove = static_cast<int32>(m_MoveRemainderX);
	if (intMove != 0)
	{
		m_MoveRemainderX -= static_cast<float>(intMove);
		return TryMoveX(intMove);
	}
	return HitResult();
}

HitResult GameObject::TryMoveY(float dy)
{
	m_MoveRemainderY += dy;
	int32 intMove = static_cast<int32>(m_MoveRemainderY);
	if (intMove != 0)
	{
		m_MoveRemainderY -= static_cast<float>(intMove);
		return TryMoveY(intMove);
	}
	return HitResult();
}

void GameObject::Destroy()
{
	if (m_Bucket)
	{
		m_Bucket->DestroyObject(this);
	}
}

void GameObject::GatherDraw(RenderManager& renderManager) const
{
	sf::Transform objectTransform;
	objectTransform.translate(ToFVec(GetTopLeft()));

	for (const std::unique_ptr<Component>& NextComponent : m_ComponentList)
	{
		NextComponent->GatherDraw(renderManager, objectTransform);
	}
}
