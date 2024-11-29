#pragma once

struct RenderLayer
{
	enum Enum
	{
		FirstLayer = 0,

		BackTiles_Lit = FirstLayer,
		MainTiles_Lit,
		BackObjects_Unlit,
		MainObjects_Lit,
		FrontEffects_Unlit,
		UI_Unlit,

		Count
	};

	static bool IsLit(Enum layer)
	{
		return layer != BackObjects_Unlit
			&& layer != FrontEffects_Unlit
			&& layer != UI_Unlit;
	}
};