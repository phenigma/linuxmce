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

	int Style_;
	int FontHeight_;
	SDL_Color FontColor;

	TTF_Font* Font_;

	bool Exists(unsigned char Letter);

public:
	GLFontTextureList(TTF_Font* Font, 
		int Style, int FontHeight,
		unsigned char R, 
		unsigned char G,
		unsigned char B);
	virtual ~GLFontTextureList();
	
	void MapLetter(char Letter);

	//returns the length in pixels
	virtual int TextOut(int Width, std::string Text, MeshContainer* &Geometry);
};

#endif /*GLFONTTEXTURELIST_H_*/
