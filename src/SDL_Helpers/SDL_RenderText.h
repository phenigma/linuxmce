#ifndef __SDL_RENDER_TEXT_H__
#define __SDL_RENDER_TEXT_H__
//---------------------------------------------------------------------------------------------------
#include "RendererImage.h"
#include "../Orbiter/DesignObj_Orbiter.h"
#include "../Orbiter/TextStyle.h"
#include "../SerializeClass/ShapesColors.h"
//---------------------------------------------------------------------------------------------------
class SDL_RenderText
{
private:
	std::string m_sFontPath;

public:

	SDL_RenderText(std::string sFontPath) : m_sFontPath(sFontPath) {}

	PlutoSize RealRenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, 
		TextStyle *pTextStyle, PlutoPoint pos);

	pair<int, int> GetWordWidth(string Word, TextStyle *pTextStyle, 
		RendererImage *& RI, bool NewSurface = true);
};
//---------------------------------------------------------------------------------------------------
#endif //__SDL_RENDER_TEXT_H__
