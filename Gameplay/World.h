#pragma once
#include "Gameplay/Gameplay.h"
#include "Gameplay/HitResult.h"
#include "Gameplay/GameSystem.h"
#include "SFML/Graphics/View.hpp"
#include "Util/Rect.h"

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

	void Draw(sf::RenderTarget& renderTarget) const;

	template<typename SystemType> SystemType* GetSystem();

	WorldGrid& GetWorldGrid();
	const WorldGrid& GetWorldGrid() const;

	void AddObject(GameObject* newObject);
	void UpdateObjectPosition(GameObject* object);
	HitResult CheckForSolid(IntRect checkRect, GameObject* ignore = nullptr) const;

	void SetMainView(sf::View& view);
	void UpdateViewport();

	int32 GetTickNumber() const { return m_TickNumber; }
	int32 GetTicksSince(int32 pastTick) const { return m_TickNumber - pastTick; }
	int32 GetTicksUntil(int32 futureTick) const { return futureTick - m_TickNumber; }

	static constexpr int32 c_InvalidFrame = -999;

protected:
	std::unique_ptr<WorldGrid> m_WorldGrid;
	std::unique_ptr<RenderManager> m_RenderManager;

	sf::View m_MainView;

	// Counter incremented at end of each world tick.
	int32 m_TickNumber = 0;

	// List of other singletons managed by World.h.  Automatically instantiated when requested.
	// Generally speaking, if the object is used in World.cpp, it should be a unique_ptr like WorldGrid.
	// If it isn't used in World.cpp, it should be a system.
	std::unordered_map<NameHash, std::unique_ptr<GameSystem>> m_SystemMap;
};

// Template implementations

template<typename SystemType>
SystemType* World::GetSystem()
{
	NameHash systemName = SystemType::StaticType();
	auto systemItr = m_SystemMap.find(systemName);
	if (systemItr == m_SystemMap.end())
	{
		auto result = m_SystemMap.emplace(systemName, new SystemType);
		if (!result.second) // insert failed
		{
			return nullptr;
		}
		systemItr = result.first;
	}
	return static_cast<SystemType*>(systemItr->second.get());
}
