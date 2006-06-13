#ifndef GLFONTTEXTURELIST_H_
#define GLFONTTEXTURELIST_H_

#include "OpenGLGraphic.h"

#include "../Mesh/MeshBuilder.h"

#include "GLFontTextureList.h"

class GLFontTextureList
{
	/**
	 * Create a texture list with all ASCII images
	 */
	OpenGLGraphic* Letters[256];
	int ScreenHeight_;

	GLFontTextureList* Font;
	
public:
	GLFontTextureList(int ScreenHeight);
	virtual ~GLFontTextureList();
	
	virtual void MapFont(
		std::string FontName,
		int Height,
		int Style,
		unsigned char R, 
		unsigned char G,
		unsigned char B
		);
	
	//returns the length in pixels
	virtual MeshContainer* TextOut(int X, int Y, char* Text);
	
};

#endif /*GLFONTTEXTURELIST_H_*/
