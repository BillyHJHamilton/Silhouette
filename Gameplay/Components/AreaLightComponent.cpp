#include "AreaLightComponent.h"

#include "App/ShaderManager.h"
#include "Gameplay/GameObject.h"

TypeInfo const AreaLightComponent::s_TypeInfo = TypeInfo("AreaLightComponent", { "Component" });

void AreaLightComponent::Tick(float deltaTime)
{
	if (m_Enabled)
	{
		ShaderManager& shaderManager = ShaderManager::Get();
		shaderManager.AddAreaLight(
			FRect(GetOwner()->GetBounds()),
			ToFVec(m_BorderSize),
			m_LightVector,
			m_Brightness * m_Colour);
	}
}
