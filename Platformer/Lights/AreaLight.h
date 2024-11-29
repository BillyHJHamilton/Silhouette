#pragma once

#include "Gameplay/GameObject.h"

class AreaLightComponent;

namespace pugi
{
	class xml_node;
}

class AreaLight : public GameObject
{
public:
	AreaLight(IntRect rect) : GameObject(rect) {}

	virtual void Init() override;

	void ParseXml(const pugi::xml_node& propertiesNode);

protected:
	AreaLightComponent* m_AreaLight = nullptr;
};
