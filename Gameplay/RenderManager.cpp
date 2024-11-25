#include "RenderManager.h"

#include "App/GameApp.h"
#include "App/ShaderManager.h"
#include "App/PerfTimer.h"
#include "SFML/Graphics/RenderTarget.hpp"

void RenderManager::AddDrawable(RenderLayer::Enum layer, const sf::Drawable* drawable,
		sf::Transform renderTransform /*= sf::Transform::Identity*/,
		float normalRotation /*= 0.0f*/, FVec normalScale /*= FVec(1.0f,1.0f)*/)
{
	assert(layer >= RenderLayer::FirstLayer && layer < RenderLayer::Count);
	m_RenderList[layer].push_back({drawable, renderTransform, normalRotation, normalScale});
}

void RenderManager::DrawAll(sf::RenderTarget& target)
{
	PerfTimer timer(__FUNCTION__);

	ShaderManager& shaderManager = GameApp::GetShaderManager();
	shaderManager.SetLightUniforms();
	sf::Shader* lightShader = shaderManager.GetLightShader();

	for (int32 layer = RenderLayer::FirstLayer; layer < RenderLayer::Count; ++layer)
	{
		if (m_RenderList[layer].empty())
		{
			continue;
		}

		const bool bIsLit = RenderLayer::IsLit((RenderLayer::Enum)layer);

		sf::RenderStates states;
		if (bIsLit)
		{
			states.shader = shaderManager.GetLightShader();
		}

		for (RenderInfo& info : m_RenderList[layer])
		{
			sf::RenderStates objectStates = states;
			objectStates.transform = info.m_RenderTransform;

			if (bIsLit)
			{
				shaderManager.SetNormalTransform(info.m_NormalRotation, info.m_NormalScale);
			}

			target.draw(*info.m_Drawable, objectStates);
		}

		m_RenderList[layer].clear();
	}

	shaderManager.ClearLights();
}
