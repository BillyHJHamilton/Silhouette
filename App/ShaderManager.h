#pragma once

#include <array>
#include "Core/BasicTypes.h"
#include "SFML/Graphics/Shader.hpp"
#include "Util/Vec2.h"

class ShaderManager
{
public:
	// Maximum number of lights that can be rendered at once.
	// Must match array sizes in fragment shader.
	static constexpr int32 c_MaxPointLights = 8;

	void LoadShaders();
	sf::Shader* GetLightShader();

	// API for gathering lights.
	// Each frame, call ClearLights, then add all the lights that should render.
	void ClearLights();
	void AddPointLight(FVec position, sf::Vector3f colour, float radius);
	void SetLightUniforms();

	void SetNormalTransform(float rotation, FVec scale);
	void ClearNormalTransform();

private:
	sf::Shader m_LightShader;

	// Light buffers for rendering.
	struct PointLights
	{
		std::array<sf::Vector2f, c_MaxPointLights> m_Position;
		std::array<sf::Vector3f, c_MaxPointLights> m_Colour;
		std::array<float, c_MaxPointLights> m_Radius;
		int32 m_Num = 0;
	};
	PointLights m_PointLights;
};
