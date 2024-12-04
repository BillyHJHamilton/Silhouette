#include "WidgetManager.h"

#include "Widget.h"
#include "Util/VectorUtil.h"

void WidgetManager::TickWidgets(float deltaTime)
{
	for (std::unique_ptr<Widget>& widgetPtr : m_WidgetList)
	{
		if (widgetPtr)
		{
			widgetPtr->Tick(deltaTime);
		}
	}

	Util::RemoveSwapAllNullItems(m_WidgetList);

	// Sort widgets by depth.
	// TODO: Is this ascending or descending?
	std::sort(m_WidgetList.begin(), m_WidgetList.end(),
		[](const std::unique_ptr<Widget>& a, const std::unique_ptr<Widget>& b)
		{
			return a->GetDepth() > b->GetDepth();
		}
	);
}

void WidgetManager::GatherDraw(RenderManager& renderManager) const
{
	for (const std::unique_ptr<Widget>& widgetPtr : m_WidgetList)
	{
		if (widgetPtr)
		{
			widgetPtr->GatherDraw(renderManager);
		}
	}
}

void WidgetManager::AddWidget(Widget* widget)
{
	assert(!Util::Contains(m_WidgetList, widget));
	m_WidgetList.emplace_back(widget);
}

void WidgetManager::DestroyWidget(Widget* widget)
{
	Util::RemoveSwapFirstMatchingItem(m_WidgetList, widget);
	assert(!Util::Contains(m_WidgetList, widget));
}
