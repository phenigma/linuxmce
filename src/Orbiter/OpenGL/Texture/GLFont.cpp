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
#include "GLFont.h"

#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#endif

#include "PlutoUtils/StringUtils.h"


GLFont::GLFont()
: Font(NULL)
{
}

GLFont::~GLFont(void)
{
	if(Font)
	{
		TTF_CloseFont(Font);
		Font = NULL;
	}  

	for(map<string, GLFontTextureList *>::iterator it = Styles.begin(),
		end = Styles.end(); it != end; ++it)
	{
		delete it->second;
	}

	Styles.clear();
}

bool GLFont::OpenFont(std::string FontName,	int Height)
{

#ifdef WIN32
#ifdef WINCE
	std::string BasePath = "C:\\Windows\\Fonts\\";
#else
	char pWindowsDirector[MAX_PATH];
	GetWindowsDirectory(pWindowsDirector, MAX_PATH);
	std::string BasePath = std::string(pWindowsDirector) + "\\Fonts\\";
#endif

#else
	string BasePath="/usr/share/fonts/truetype/msttcorefonts/";
#endif //win32	

	std::string sPathFont = BasePath + FontName + ".ttf";

	Font = TTF_OpenFont(sPathFont.c_str(), Height);
	Height_ = Height;

	return Font != NULL;
}

void GLFont::CloseFont()
{
	if(Font == NULL)
		return;
	TTF_CloseFont(Font);
}

bool GLFont::Exists(std::string Style)
{
	return (Styles.find(Style) != Styles.end());
}

std::string GLFont::StyleToString(int R, int G, int B, int Style)
{
	std::string Result = 
		StringUtils::ltos(R) + ","+ 
		StringUtils::ltos(G) + ","+ 
		StringUtils::ltos(B) + "~"+ 
		StringUtils::ltos(Style);
	return Result;
}

GLFontTextureList* GLFont::GetFontStyle(int Style, int R, int G, int B)
{
	std::string StrStyle = StyleToString(R, G, B, Style);
	if(Exists(StrStyle))
		return Styles[StrStyle];
	Styles[StrStyle] = new GLFontTextureList(Font, Style, Height_, R, G, B);

	return Styles[StrStyle];
}
