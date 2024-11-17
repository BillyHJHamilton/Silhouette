#include "GameObject.h"
#include "Gameplay/WorldGrid/ObjectBucket.h"
#include "Gameplay/World.h"
#include "Util/Math.h"

GameObject::GameObject()
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

void GameObject::Tick(float deltaTime)
{
	assert(m_World != nullptr);
	assert(m_Bucket != nullptr);

	// Tick components
	/*for (std::unique_ptr<Component>& NextComponent : m_ComponentList)
	{
		NextComponent->Tick(deltaTime);
	}*/
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
		sf::IntRect testRect = m_Bounds;
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
		sf::IntRect testRect = m_Bounds;
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

void GameObject::Destroy()
{
	if (m_Bucket)
	{
		m_Bucket->DestroyObject(this);
	}
}
