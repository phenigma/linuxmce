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
#ifndef GLFont_H_
#define GLFont_H_

#include <SDL_ttf.h>
#include <string>

#include "GLFontTextureList.h"

class GLFont
{
	TTF_Font * Font;

	std::map <std::string, GLFontTextureList*> Styles;

	bool Exists(std::string Style);
	std::string StyleToString(int R, int G, int B, int Style);

	int Height_;
public:
	/**
	 *	Default constructor
	 */
	GLFont();
	/**
	 *	Default destructor
	 */
	~GLFont(void);

	/**
	 *	Tries to open a specified font
	 */
	bool OpenFont(std::string FontName,	int Height);

	/**
	 *	Cleanup font resources and styles
	 */
	void CloseFont();

	/**
	 *	Creates the font style that match the desciption (or reuse one existing)
	 */
	GLFontTextureList* GetFontStyle(int Style, int R, int G, int B);


};

#endif
