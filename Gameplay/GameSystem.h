#pragma once

#include "Core/TypeInfo.h"

class GameSystem : public ITypeInfoProvider
{
public:
	GameSystem();
	virtual ~GameSystem();
	MACRO_DeclareTypeInfo(GameSystem)

	virtual void Tick(float deltaTime) {};

#if DEBUG_MEMORY
	static void CheckMemoryReleased();
#endif

private:

#if DEBUG_MEMORY
	static int s_NumCreated;
	static int s_NumDestroyed;
#endif
};
