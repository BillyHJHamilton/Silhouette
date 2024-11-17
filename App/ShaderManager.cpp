#include <iostream>
#include "ShaderManager.h"

void ShaderManager::LoadShaders()
{
	const bool loaded = m_LightShader.loadFromFile(
		"Resources/Shaders/LightShader.vert",
		"Resources/Shaders/LightShader.frag");
	if (!loaded)
    {
        std::cerr << "Failed to load light shader" << std::endl;
    }
}

sf::Shader* ShaderManager::GetLightShader()
{
    return &m_LightShader;
}

void ShaderManager::ClearLights()
{
	m_PointLights.m_Num = 0;
}

void ShaderManager::AddPointLight(sf::Vector2f position, sf::Vector3f colour, float radius)
{
	if (m_PointLights.m_Num < c_MaxPointLights)
	{
		m_PointLights.m_Position[m_PointLights.m_Num] = position;
		m_PointLights.m_Colour[m_PointLights.m_Num] = colour;
		m_PointLights.m_Radius[m_PointLights.m_Num] = radius;

		++m_PointLights.m_Num;
	}
	else
	{
		std::cerr << "Maximum lights exceeded." << std::endl;
	}
}

void ShaderManager::SetLightUniforms()
{
	m_LightShader.setUniformArray("pointLightPosition", m_PointLights.m_Position.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightColour", m_PointLights.m_Colour.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightRadius", m_PointLights.m_Radius.data(), c_MaxPointLights);
	m_LightShader.setUniform("pointLightNum", m_PointLights.m_Num);
}
