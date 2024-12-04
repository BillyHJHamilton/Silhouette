#include "ShaderManager.h"

#include <iostream>
#include "App/GameApp.h"
#include "App/PerfTimer.h"
#include "SFML/Graphics/Transformable.hpp"
#include "Util/Math.h"

ShaderManager& ShaderManager::Get()
{
	return GameApp::GetShaderManager();
}

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
	m_AreaLights.m_Num = 0;
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
		std::cerr << "Maximum point lights exceeded." << std::endl;
	}
}

void ShaderManager::AddAreaLight(FRect area, sf::Vector2f borderSize, sf::Vector2f lightVector, sf::Vector3f colour)
{
	if (m_AreaLights.m_Num < c_MaxAreaLights)
	{
		m_AreaLights.m_Centre[m_AreaLights.m_Num] = {area.left + 0.5f*area.width, area.top + 0.5f*area.height};
		m_AreaLights.m_Extent[m_AreaLights.m_Num] = {0.5f * area.width, 0.5f * area.height};
		m_AreaLights.m_BorderSize[m_AreaLights.m_Num] = { std::max(borderSize.x, 1.0f), std::max(borderSize.y, 1.0f) };
		m_AreaLights.m_LightVector[m_AreaLights.m_Num] = lightVector;
		m_AreaLights.m_Colour[m_AreaLights.m_Num] = colour;

		++m_AreaLights.m_Num;
	}
	else
	{
		std::cerr << "Maximum area lights exceeded." << std::endl;
	}
}

void ShaderManager::SetLightUniforms()
{
	PerfTimer timer(__FUNCTION__);

	m_LightShader.setUniformArray("pointLightPosition", m_PointLights.m_Position.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightColour", m_PointLights.m_Colour.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightRadius", m_PointLights.m_Radius.data(), c_MaxPointLights);
	m_LightShader.setUniform("pointLightNum", m_PointLights.m_Num);

	m_LightShader.setUniformArray("areaLightCentre", m_AreaLights.m_Centre.data(), c_MaxAreaLights);
	m_LightShader.setUniformArray("areaLightExtent", m_AreaLights.m_Extent.data(), c_MaxAreaLights);
	m_LightShader.setUniformArray("areaLightBorder", m_AreaLights.m_BorderSize.data(), c_MaxAreaLights);
	m_LightShader.setUniformArray("areaLightVector", m_AreaLights.m_LightVector.data(), c_MaxAreaLights);
	m_LightShader.setUniformArray("areaLightColour", m_AreaLights.m_Colour.data(), c_MaxAreaLights);
	m_LightShader.setUniform("areaLightNum", m_AreaLights.m_Num);
}

void ShaderManager::SetNormalTransform(float rotation, FVec scale)
{
	sf::Transformable transform;
	transform.setScale(Math::Sign(scale.x), Math::Sign(scale.y));
	transform.setRotation(rotation);

	// Extract a 2x2 part of the matrix as two Vec2's.
	// Annoyingly, SFML has no overload for mat2 uniforms.
	const float* matrix = transform.getTransform().getMatrix();
	FVec row0(matrix[0], matrix[1]);
	FVec row1(matrix[4], matrix[5]);
	m_LightShader.setUniform("normalTransform0", row0);
	m_LightShader.setUniform("normalTransform1", row1);
}

void ShaderManager::ClearNormalTransform()
{
	m_LightShader.setUniform("normalTransform0", FVec(1.0,0.0));
	m_LightShader.setUniform("normalTransform1", FVec(0.0,1.0));
}

