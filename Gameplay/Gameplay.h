#pragma once

#include "Core/Reference.h"
#include <vector>

// Some useful forward declarations.
class Component;
class GameObject;
class GameSystem;
class ObjectBucket;
class RenderManager;
class TilePatch;
class World;
class WorldGrid;
class WorldGridCell;

struct Tileset;

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

