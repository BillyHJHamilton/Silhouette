#pragma once
#include "Gameplay/Gameplay.h"
#include "Gameplay/HitResult.h"
#include "Gameplay/WorldGrid/Tileset.h"
#include "Gameplay/WorldGrid/WorldGrid.h"
#include "Gameplay/GameSystem.h"

namespace sf
{
	class RenderTarget;
}

class World
{
public:
	World();
	virtual ~World();

	void Init();
	void Tick(float deltaTime);

	// Perform tasks before rendering, such as collecting lights.
	//void PreRender();

	void Draw(sf::RenderTarget& renderTarget) const;

	template<typename SystemType> SystemType* GetSystem();

	WorldGrid& GetWorldGrid();
	const WorldGrid& GetWorldGrid() const;

	void AddObject(GameObject* newObject);
	void UpdateObjectPosition(GameObject* object);
	HitResult CheckForSolid(sf::IntRect checkRect, GameObject* ignore = nullptr) const;

protected:
	std::unique_ptr<WorldGrid> m_WorldGrid;

	// List of other singletons managed by World.h.  Automatically instantiated when requested.
	// Generally speaking, if the object is used in World.cpp, it should be a unique_ptr like WorldGrid.
	// If it isn't used in World.cpp, it should be a system.
	std::unordered_map<NameHash, std::unique_ptr<GameSystem>> m_SystemMap;
};
