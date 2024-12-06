#include "RenderLayer.h"

RenderLayer::Info RenderLayer::s_Info[RenderLayer::Enum::Count] = 
{							//	Shader			View
/*BackTiles_Lit*/			{	LitShader,		MainView	},
/*MainTiles_Lit*/			{	LitShader,		MainView	},
/*BackObjects_Unlit*/		{	DefaultShader,	MainView	},
/*MainObjects_Lit*/			{	LitShader,		MainView	},
/*FrontEffects_Unlit*/		{	DefaultShader,	MainView	},
/*FrontEffects_HitFlash*/	{  HitFlashShader,	MainView	},
/*UI_Unlit*/				{	DefaultShader,	WindowView	},
/*UI_ScreenFade*/			{	DefaultShader,	WindowView	}
};

RenderLayer::ShaderType RenderLayer::GetShaderType(Enum layer)
{
	return s_Info[layer].m_ShaderType;
}

RenderLayer::ViewType RenderLayer::GetViewType(Enum layer)
{
	return s_Info[layer].m_ViewType;
}
