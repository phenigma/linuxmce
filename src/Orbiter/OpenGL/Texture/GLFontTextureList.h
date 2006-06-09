#ifndef GLFONTTEXTURELIST_H_
#define GLFONTTEXTURELIST_H_

#include "OpenGLGraphic.h"

#include "../Mesh/MeshBuilder.h"

class GLFontTextureList
{
	/**
	 * Create a texture list with all ASCII images
	 */
	OpenGLGraphic* Letters[256];
public:
	GLFontTextureList();
	virtual ~GLFontTextureList();
	
	virtual void MapFont(
		std::string FontName,
		int Height, 
		int Style
		);
	
	//returns the length in pixels
	virtual MeshContainer* TextOut(int X, int Y, char* Text);
	
};

#endif /*GLFONTTEXTURELIST_H_*/
