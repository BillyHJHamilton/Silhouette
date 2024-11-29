#include "AreaLight.h"

#include "External/Pugixml/pugixml.hpp"
#include "Gameplay/Components/AreaLightComponent.h"
#include "SFML/Graphics/Color.hpp"
#include "Util/ColourUtil.h"

void AreaLight::Init()
{
	m_AreaLight = EmplaceComponent<AreaLightComponent>();
}

void AreaLight::ParseXml(const pugi::xml_node& propertiesNode)
{
	pugi::xml_node borderXNode = propertiesNode.find_child_by_attribute("name", "BorderX");
	pugi::xml_node borderYNode = propertiesNode.find_child_by_attribute("name", "BorderY");
	pugi::xml_node brightnessNode = propertiesNode.find_child_by_attribute("name", "Brightness");
	pugi::xml_node colourNode = propertiesNode.find_child_by_attribute("name", "Colour");
	pugi::xml_node vectorXNode = propertiesNode.find_child_by_attribute("name", "VectorX");
	pugi::xml_node vectorYNode = propertiesNode.find_child_by_attribute("name", "VectorY");

	IntVec border = {borderXNode.attribute("value").as_int(1), borderYNode.attribute("value").as_int(1)};
	float brightness = brightnessNode.attribute("value").as_float(1.0f);
	std::string colourString = colourNode.attribute("value").as_string();
	sf::Color colour = colourString.empty() ? sf::Color() : Util::ParseTiledColor(colourString);
	sf::Vector3f colourVec = Util::ColourToFVec3(colour);
	FVec vector = {vectorXNode.attribute("value").as_float(), vectorYNode.attribute("value").as_float()};

	m_AreaLight->SetBorderSize(border);
	m_AreaLight->SetBrightness(brightness);
	m_AreaLight->SetColour(colourVec);
	m_AreaLight->SetLightVector(vector);
}
