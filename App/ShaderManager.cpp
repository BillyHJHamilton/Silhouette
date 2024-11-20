#include "ShaderManager.h"

#include <iostream>
#include "App/PerfTimer.h"
#include "SFML/Graphics/Transformable.hpp"
#include "Util/Math.h"

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
	PerfTimer timer(__FUNCTION__);

	m_LightShader.setUniformArray("pointLightPosition", m_PointLights.m_Position.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightColour", m_PointLights.m_Colour.data(), c_MaxPointLights);
	m_LightShader.setUniformArray("pointLightRadius", m_PointLights.m_Radius.data(), c_MaxPointLights);
	m_LightShader.setUniform("pointLightNum", m_PointLights.m_Num);
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

