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
#include "GLFontManager.h"

#include "PlutoUtils/StringUtils.h"

/*static*/ GLFontManager* GLFontManager::Instance_ = NULL;

/*static*/ GLFontManager* GLFontManager::GetInstance()
{
	if(Instance_ == NULL)
		Instance_ = new GLFontManager();
	return Instance_;
}

GLFontManager::~GLFontManager()
{
	for(map<std::string, GLFont*>::iterator it = Fonts.begin(), 
		end = Fonts.end(); it != end; ++it)
	{
		delete it->second;
	}

	Fonts.clear();
}

void GLFontManager::CleanUp()
{
	delete Instance_;
	Instance_ = NULL;
}

bool GLFontManager::Exists(std::string FontStr)
{
	return (Fonts.find(FontStr) != Fonts.end());
}

std::string GLFontManager::FontToString(std::string FontName, int Height)
{
	std::string Result= FontName + "@" + StringUtils::ltos(Height);
	return Result;
}

GLFont* GLFontManager::GetFont(std::string FontName, int Height)
{
	std::string FontStr = FontToString(FontName, Height);
	if(Exists(FontStr))
		return Fonts[FontStr];
	GLFont* Result = new GLFont(); 
	Result->OpenFont(FontName, Height);
	Fonts[FontStr] = Result;
	return Result;
}