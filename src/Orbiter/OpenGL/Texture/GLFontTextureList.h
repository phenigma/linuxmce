#ifndef GLFONTTEXTURELIST_H_
#define GLFONTTEXTURELIST_H_

#include "../OpenGLGraphic.h"

#include "../Mesh/MeshBuilder.h"

#include "GLFontTextureList.h"

#include <map>
#include <SDL_ttf.h>

class GLFontTextureList
{
	/**
	 * Create a texture list with all ASCII images
	 */
	std::map<char, OpenGLGraphic*> Letters;
	int ScreenHeight_;

	int Style_;
	int FontHeight_;
	SDL_Color FontColor;

	TTF_Font* Font_;

	bool Exists(unsigned char Letter);

public:
	GLFontTextureList(int ScreenHeight, TTF_Font* Font, 
		int Style, int FontHeight,
		unsigned char R, 
		unsigned char G,
		unsigned char B);
	virtual ~GLFontTextureList();
	
	void MapLetter(char Letter);

	//returns the length in pixels
	virtual MeshContainer* TextOut(int X, int Y, char* Text);

};

#endif /*GLFONTTEXTURELIST_H_*/
