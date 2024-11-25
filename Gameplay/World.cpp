#include "World.h"
#include "App/GameApp.h"
#include "App/PerfTimer.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/WorldGrid/WorldGrid.h"
#include "Gameplay/RenderManager.h"

World::World()
{
	m_WorldGrid = std::make_unique<WorldGrid>();
	m_RenderManager = std::make_unique<RenderManager>();
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

	// Tick game objects
	IntRect tickArea(0, 0, 640, 480); // TODO
	m_WorldGrid->TickObjects(deltaTime, tickArea);

	++m_TickNumber;
}

void World::Draw(sf::RenderTarget& renderTarget) const
{
	// Draw area in main view, plus neighbouring patches (in case large sprites are spilling over).
	const FVec viewTopLeft = m_MainView.getCenter() - 0.5f*m_MainView.getSize();
	const IntVec drawTopLeft = RoundToIntVec(viewTopLeft - 0.5f*ToFVec(c_PatchWidth, c_PatchHeight));
	const IntVec drawSize = RoundToIntVec(m_MainView.getSize()) + 2*IntVec(c_PatchWidth, c_PatchHeight);
	const IntRect m_DrawRect(drawTopLeft, drawSize);

	renderTarget.setView(m_MainView);
	m_WorldGrid->GatherDraw(*m_RenderManager, m_DrawRect);
	m_RenderManager->DrawAll(renderTarget);
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
