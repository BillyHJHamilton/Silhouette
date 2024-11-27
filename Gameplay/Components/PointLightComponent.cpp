#include "PointLightComponent.h"

#include "App/GameApp.h"
#include "App/ShaderManager.h"
#include "Gameplay/GameObject.h"

TypeInfo const PointLightComponent::s_TypeInfo = TypeInfo("PointLightComponent", {"Component"});

void PointLightComponent::Tick(float deltaTime)
{
	if (m_Enabled)
	{
		ShaderManager &shaderManager = GameApp::GetShaderManager();
		shaderManager.AddPointLight(
			ToFVec(GetOwner()->GetTopLeft() + m_Offset),
			m_Brightness * m_Colour,
			m_Radius);
	}
}
