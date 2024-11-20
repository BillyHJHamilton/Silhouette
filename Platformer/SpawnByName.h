#pragma once

#include "Core/NameHash.h"
#include "Gameplay/Gameplay.h"
#include "Util/Vec2.h"

// Master factory method.
void SpawnByName(World& world, NameHash name, IntVec position);
