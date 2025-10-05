#include "Rect.h"
#include "Vec2.h"

namespace Util
{
	IntRect PadRect(IntRect rect, int paddingX, int paddingY)
	{
		return IntRect(rect.left - paddingX, rect.top - paddingY, rect.width + 2*paddingX, rect.height + 2*paddingY);
	}

	FRect PadRect(FRect rect, float paddingX, float paddingY)
	{
		return FRect(rect.left - paddingX, rect.top - paddingY, rect.width + 2.0f*paddingX, rect.height + 2.0f*paddingY);
	}
}
