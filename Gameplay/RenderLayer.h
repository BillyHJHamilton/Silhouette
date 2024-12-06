#pragma once

#include "Core/BasicTypes.h"

struct RenderLayer
{
	enum Enum : uint8
	{
		FirstLayer = 0,

		BackTiles_Lit = FirstLayer,
		MainTiles_Lit,
		BackObjects_Unlit,
		MainObjects_Lit,
		FrontEffects_Unlit,
		FrontEffects_HitFlash,
		UI_Unlit,
		UI_ScreenFade,

		Count
	};

	enum ShaderType : uint8
	{
		DefaultShader,
		LitShader,
		HitFlashShader
	};

	enum ViewType : uint8
	{
		MainView,
		WindowView
	};

	static ShaderType GetShaderType(Enum layer);
	static ViewType GetViewType(Enum layer);

private:
	struct Info
	{
		ShaderType m_ShaderType = DefaultShader;
		ViewType m_ViewType = MainView;
	};
	static Info s_Info [Enum::Count];
};