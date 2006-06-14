#ifndef GLFONTRENDERER_H_
#define GLFONTRENDERER_H_

#include<string>
#include<vector>
#include "../../Orbiter.h"

#include "../Mesh/MeshFrame.h"
//#include "GLFontTextureList.h"
#include "GLFont.h"

class GLFontRenderer
{
	std::vector <MeshFrame*> MeshWords;
	std::vector <std::string> Words;

	void WordCutter();

	int Style_, Height_;
	unsigned char R_, G_, B_; 

	GLFont* Font;

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
		bool bItalic,bool bUnderline, PlutoPoint point, class PlutoRectangle &rectCalcLocation);
	
	MeshFrame* TextOut(std::string &TextToDisplay,class DesignObjText *Text,
		class TextStyle *pTextStyle, class PlutoPoint point);
	
};

#endif /*GLFONTRENDERER_H_*/
