#ifndef GLFONTRENDERER_H_
#define GLFONTRENDERER_H_

#include<string>
#include<vector>
#include "../Mesh/MeshFrame.h"
#include "GLFontTextureList.h"
#include "../../Orbiter.h"

class GLFontRenderer
{
	std::vector <MeshFrame*> MeshWords;
	std::vector <std::string> Words;

	void WordCutter();
	GLFontTextureList Font;
public:
	/**
	 * Utility class that render a font and create a mesh to be drawn.
	 */
	GLFontRenderer(
		int ScreenHeight,
		std::string FontName,
		int Height, 
		int Style,
		unsigned char R, 
		unsigned char G,
		unsigned char B);
	virtual ~GLFontRenderer();
	DCE::Orbiter* OrbiterLogic();

	MeshFrame* RenderText(string &TextToDisplay, PlutoRectangle &rPosition, int iPK_HorizAlignment, 
		int iPK_VertAlignment, string &sFont, PlutoColor &ForeColor, int iPixelHeight, bool bBold,
		bool bItalic,bool bUnderline, PlutoPoint point, struct RECT &rectCalcLocation);
	
	MeshFrame* TextOut(std::string TextToDisplay, DesignObjText *Text, TextStyle* pTextStyle, 
		PlutoPoint point, RECT &rectLocation);
	
};

#endif /*GLFONTRENDERER_H_*/
