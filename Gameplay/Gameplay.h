#pragma once

#include "Core/Reference.h"
#include <vector>

// Some useful forward declarations.
class Component;
class GameObject;
class GameSystem;
class ObjectBucket;
class World;
class WorldGrid;
class WorldGridCell;

using ObjectRef = WeakRef<GameObject>;
using ObjectConstRef = WeakRef<const GameObject>;
using ObjectRefList = std::vector<ObjectRef>;
using ObjectConstRefList = std::vector<ObjectConstRef>;

namespace sf
{
	class RenderStates;
	class RenderTarget;
	class Sprite;
	class Texture;
}

