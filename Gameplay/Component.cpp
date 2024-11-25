#include "Component.h"

#include "GameObject.h"

TypeInfo const Component::s_TypeInfo = TypeInfo("Component", {});

Component::Component()
{
	#if DEBUG_MEMORY
		++s_NumCreated;
	#endif
}

Component::~Component()
{
	#if DEBUG_MEMORY
		++s_NumDestroyed;
	#endif
}

#if DEBUG_MEMORY
int Component::s_NumCreated = 0;
int Component::s_NumDestroyed = 0;

void Component::CheckMemoryReleased()
{
	if (s_NumCreated == s_NumDestroyed)
	{
		std::cout << "Component Memory OK." << std::endl;
	}
	else
	{
		std::cerr << "Component Memory Error!  Created " << s_NumCreated
			<< ", Destroyed " << s_NumDestroyed << std::endl;
	}
}
#endif

const World* Component::GetWorld() const
{
	assert(GetOwner());
	return GetOwner()->GetWorld();
}

World* Component::GetWorld()
{
	assert(GetOwner());
	return GetOwner()->GetWorld();
}

void Component::SetOwner(GameObject* newOwner)
{
	m_Owner = newOwner;
	if (newOwner)
	{
		OnAddedToObject(*newOwner);
	}
}
