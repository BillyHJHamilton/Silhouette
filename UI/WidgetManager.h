#pragma once

#include <memory>
#include <vector>

class RenderManager;
class Widget;

class WidgetManager
{
public:
	void TickWidgets(float deltaTime);
	void GatherDraw(RenderManager& renderManager) const;

	void AddWidget(Widget* widget);
	void DestroyWidget(Widget* widget);

private:
	std::vector<std::unique_ptr<Widget>> m_WidgetList;
};
