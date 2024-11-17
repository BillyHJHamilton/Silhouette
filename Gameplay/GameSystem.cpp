#include "GameSystem.h"

TypeInfo const GameSystem::s_TypeInfo = TypeInfo("GameSystem", {});

GameSystem::GameSystem()
{
	#if DEBUG_MEMORY
		++s_NumCreated;
	#endif
}

GameSystem::~GameSystem()
{
	#if DEBUG_MEMORY
		++s_NumDestroyed;
	#endif
}

#if DEBUG_MEMORY
int GameSystem::s_NumCreated = 0;
int GameSystem::s_NumDestroyed = 0;

void GameSystem::CheckMemoryReleased()
{
	if (s_NumCreated == s_NumDestroyed)
	{
		std::cout << "System Memory OK." << std::endl;
	}
	else
	{
		std::cerr << "System Memory Error!  Created " << s_NumCreated
			<< ", Destroyed " << s_NumDestroyed << std::endl;
	}
}
#endif
