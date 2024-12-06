#include "RenderManager.h"

#include "App/PerfTimer.h"
#include "App/ShaderManager.h"
#include "App/WindowManager.h"
#include "SFML/Graphics/RenderTarget.hpp"

void RenderManager::AddDrawable(RenderLayer::Enum layer, const sf::Drawable* drawable,
		sf::Transform renderTransform /*= sf::Transform::Identity*/,
		float normalRotation /*= 0.0f*/, FVec normalScale /*= FVec(1.0f,1.0f)*/)
{
	assert(layer >= RenderLayer::FirstLayer && layer < RenderLayer::Count);
	m_RenderList[layer].push_back({drawable, renderTransform, normalRotation, normalScale});
}

void RenderManager::DrawAll(sf::RenderTarget& target, sf::View mainView)
{
	PerfTimer timer(__FUNCTION__);

	ShaderManager &shaderManager = ShaderManager::Get();
	shaderManager.SetLightUniforms();
	shaderManager.SetHitFlashUniforms();

	sf::View windowView = WindowManager::Get().GetNativeView();
	windowView.setViewport(CalculateViewport(windowView));
	mainView.setViewport(CalculateViewport(mainView));

	for (int32 layer = RenderLayer::FirstLayer; layer < RenderLayer::Count; ++layer)
	{
		if (m_RenderList[layer].empty())
		{
			continue;
		}

		const RenderLayer::ShaderType shaderType = RenderLayer::GetShaderType((RenderLayer::Enum)layer);
		const RenderLayer::ViewType viewType = RenderLayer::GetViewType((RenderLayer::Enum)layer);

		sf::RenderStates states;
		if (shaderType == RenderLayer::LitShader)
		{
			states.shader = shaderManager.GetLightShader();
		}
		else if (shaderType == RenderLayer::HitFlashShader)
		{
			states.shader = shaderManager.GetHitFlashShader();
		}

		if (viewType == RenderLayer::MainView)
		{
			target.setView(mainView);
		}
		else if (viewType == RenderLayer::WindowView)
		{
			target.setView(windowView);
		}

		for (RenderInfo& info : m_RenderList[layer])
		{
			sf::RenderStates objectStates = states;
			objectStates.transform = info.m_RenderTransform;

			if (shaderType == RenderLayer::LitShader)
			{
				shaderManager.SetNormalTransform(info.m_NormalRotation, info.m_NormalScale);
			}

			target.draw(*info.m_Drawable, objectStates);
		}

		m_RenderList[layer].clear();
	}
}

FRect RenderManager::CalculateViewport(sf::View& view)
{
	const float screenRatio = WindowManager::Get().GetScreenRatio();
	const FVec viewSize = view.getSize();
	const float viewRatio = viewSize.x/viewSize.y;

	const float viewScreenRatio = viewRatio / screenRatio;
	if (viewScreenRatio < 1.0f) // Need to letterbox on left/right
	{
		const float padding = (1.0f - viewScreenRatio)*0.5f;
		return FRect(padding, 0.0f, viewScreenRatio, 1.0f);
	}
	else if (viewScreenRatio > 1.0f) // Need to letterbox on top/bottom
	{
		const float inverseRatio = 1.0f/viewScreenRatio;
		const float padding = (1.0f - inverseRatio)*0.5f;
		return FRect(0.0f, padding, 1.0f, inverseRatio);
	}
	else
	{
		return FRect(0.0f, 0.0f, 1.0f, 1.0f);
	}
}
