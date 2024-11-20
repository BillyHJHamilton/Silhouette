#include "World.h"
#include "App/GameApp.h"
#include "App/PerfTimer.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/WorldGrid/WorldGrid.h"

World::World()
{
	m_WorldGrid = std::make_unique<WorldGrid>();
}

World::~World()
{
}

void World::Init()
{
	m_WorldGrid->BuildVertexArrays();
}

void World::Tick(float deltaTime)
{
	PerfTimer timer(__FUNCTION__);

	ShaderManager& shaderManager = GameApp::GetShaderManager();
	shaderManager.ClearLights();

	// Tick game objects
	IntRect tickArea(0, 0, 640, 480); // TODO
	m_WorldGrid->TickObjects(deltaTime, tickArea);

	{
		shaderManager.AddPointLight( {100.0f, 100.0f}, {1.0f, 0.0f, 0.0f}, 300.0f);
		shaderManager.AddPointLight( {300.0f, 150.0f}, {1.0f, 0.8f, 0.0f}, 300.0f);
		shaderManager.AddPointLight( {200.0f, 500.0f}, {0.0f, 0.0f, 1.0f}, 300.0f);
		shaderManager.SetLightUniforms();
	}

	++m_TickNumber;
}

void World::Draw(sf::RenderTarget& renderTarget) const
{
	sf::RenderStates states;

	ShaderManager& shaderManager = GameApp::GetShaderManager();
	if (sf::Shader* lightShader = shaderManager.GetLightShader())
	{
		states.shader = lightShader;
	}

	renderTarget.setView(m_MainView);
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

HitResult World::CheckForSolid(IntRect checkRect, GameObject* ignore) const
{
	return m_WorldGrid->CheckForSolid(checkRect, ignore);
}

void World::AddObject(GameObject* newObject)
{
	newObject->SetWorld(this);
	m_WorldGrid->AddObject(newObject);
	newObject->Init();
}

void World::UpdateObjectPosition(GameObject* object)
{
	m_WorldGrid->UpdateObjectPosition(object);
}

void World::SetMainView(sf::View& view)
{
	m_MainView = view;
	UpdateViewport();
}

void World::UpdateViewport()
{
	const float screenRatio = GameApp::Get().GetScreenRatio();
	const FVec viewSize = m_MainView.getSize();
	const float viewRatio = viewSize.x/viewSize.y;

	const float viewScreenRatio = viewRatio / screenRatio;
	if (viewScreenRatio < 1.0f) // Need to letterbox on left/right
	{
		const float padding = (1.0f - viewScreenRatio)*0.5f;
		m_MainView.setViewport(FRect(padding, 0.0f, viewScreenRatio, 1.0f));
	}
	else if (viewScreenRatio > 1.0f) // Need to letterbox on top/bottom
	{
		const float inverseRatio = 1.0f/viewScreenRatio;
		const float padding = (1.0f - inverseRatio)*0.5f;
		m_MainView.setViewport(FRect(0.0f, padding, 1.0f, inverseRatio));
	}
}
