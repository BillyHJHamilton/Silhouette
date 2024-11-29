#pragma once

#include "Core/NameHash.h"
#include "Gameplay/Gameplay.h"
#include "Util/Rect.h"
#include "Util/Vec2.h"

namespace pugi
{
	class xml_node;
}

// Master factory methods.
void SpawnByName(World& world, NameHash name, IntVec position);
void SpawnAreaByName(World& world, NameHash name, IntRect area, const pugi::xml_node& propertiesNode);
