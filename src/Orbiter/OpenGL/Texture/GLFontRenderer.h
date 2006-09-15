/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
#ifndef GLFONTRENDERER_H_
#define GLFONTRENDERER_H_

#include<string>
#include<vector>
#include "../../Orbiter.h"

#include "../Mesh/MeshFrame.h"
//#include "GLFontTextureList.h"
#include "GLFont.h"

/**
 * Utility class that render a font and create a mesh to be drawn.
 */
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
	 *	Default constructor 
	 */
	GLFontRenderer(
		int ScreenHeight,
		std::string FontName,
		int Height, 
		int Style,
		unsigned char R, 
		unsigned char G,
		unsigned char B);
	/**
	 *	Default destructor 
	 */
	virtual ~GLFontRenderer();
	/**
	 *	draws a text on a specified position 
	 */
	MeshFrame* RenderText(string TextUniqueID, string& TextToDisplay, PlutoRectangle &rPosition, int iPK_HorizAlignment, 
		int iPK_VertAlignment, string &sFont, PlutoColor &ForeColor, int iPixelHeight, bool bBold,
		bool bItalic,bool bUnderline, class PlutoRectangle &rectCalcLocation);
	
	/**
	 *	draws a text on a specified position 
	 */
	MeshFrame* TextOut(string TextUniqueID, string TextToDisplay,class DesignObjText *Text,
		class TextStyle *pTextStyle, class PlutoPoint point);
	
};

#endif /*GLFONTRENDERER_H_*/
