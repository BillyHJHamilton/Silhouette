#include "SpawnByName.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/World.h"
#include "Platformer/Player.h"
#include "Platformer/Lights/StrobeLight.h"

void SpawnByName(World& world, NameHash name, IntVec position)
{
	switch(name)
	{
		case NameHash::StaticHash("Player"):
		{
			world.AddObject(new Player(position));
			break;
		}
		case NameHash::StaticHash("StrobeLight"):
		{
			world.AddObject(new StrobeLight(position));
			break;
		}
		default:
		{
			#if DEBUG_NAME_HASHING
				std::cerr << "Unknown label in SpawnByName: " << name.GetNameString() << std::endl;
			#else
				std::cerr << "Unknown label in SpawnByName." << std::endl;
			#endif
			break;
		}
	}
}
