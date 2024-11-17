#include "World.h"
#include "App/GameApp.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/WorldGrid/Tileset.h"
#include "Gameplay/WorldGrid/LoadWorld.h"
#include "Util/Math.h"
#include "Util/Random.h"

World::World()
{
	m_WorldGrid = std::make_unique<WorldGrid>();
}

World::~World()
{
}

void World::Init()
{
/*	for (int32 y = 0; y < 600; y += 16)
	{
		for (int32 x = 0; x < 450; x += 16)
		{
			if (Random::Coinflip())
			{
				int32 tileId = Random::IntInRange(0,3);
				m_WorldGrid->AddTile({x,y}, tileId);
			}
		}
	}*/

	LoadTilesFromCsv("Resources/Maps/Map1.csv", *m_WorldGrid);

	m_WorldGrid->BuildVertexArrays();
}

void World::Tick(float deltaTime)
{
	// Tick game objects
	sf::IntRect tickArea; // TODO
	m_WorldGrid->TickObjects(deltaTime, tickArea);
}

void World::Draw(sf::RenderTarget& renderTarget) const
{
	RenderStates states;

	ShaderManager& shaderManager = GameApp::GetShaderManager();
	if (Shader* lightShader = shaderManager.GetLightShader())
	{
		shaderManager.ClearLights();
		shaderManager.AddPointLight( {100.0f, 100.0f}, {1.0f, 0.0f, 0.0f}, 300.0f);
		shaderManager.AddPointLight( {300.0f, 150.0f}, {1.0f, 0.8f, 0.0f}, 300.0f);
		shaderManager.AddPointLight( {200.0f, 500.0f}, {0.0f, 0.0f, 1.0f}, 300.0f);
		shaderManager.SetLightUniforms();

		states.shader = lightShader;
	}

	renderTarget.draw(*m_WorldGrid, states);
}

WorldGrid& World::GetWorldGrid()
{
	assert(m_WorldGrid);
	return *m_WorldGrid;
}

const WorldGrid& World::GetWorldGrid() const
{
	assert(m_WorldGrid);
	return *m_WorldGrid;
}

HitResult World::CheckForSolid(sf::IntRect checkRect, GameObject* ignore) const
{
	return m_WorldGrid->CheckForSolid(checkRect, ignore);
}

void World::AddObject(GameObject* newObject)
{
	newObject->SetWorld(this);
	m_WorldGrid->AddObject(newObject);
}

void World::UpdateObjectPosition(GameObject* object)
{
	m_WorldGrid->UpdateObjectPosition(object);
}
