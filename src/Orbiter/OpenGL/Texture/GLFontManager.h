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
#ifndef GLFontManager_H_
#define GLFontManager_H_

#include "GLFont.h"

/**
 *	Class that keeps fonts that are identified with name and size 
 */
class GLFontManager
{
	std::map <std::string, GLFont*> Fonts;

	/**
	 *	Generates a key to identify the fonts
	 */
	std::string FontToString(std::string FontName, int Height);

	static GLFontManager* Instance_;
public:
	/**
	 *	Destructor
	 */
	~GLFontManager();

	/**
	 *	Getter for singleton instance
	 */
	static GLFontManager* GetInstance();

	/**
	 *	Cleanup the fonts are used
	 */
	void CleanUp();

	/**
	 *	Check if exists the 
	 */
	bool Exists(std::string FontStr);
	/**
	 *	Get (a new or existing) font that match with name and width
	 */
	GLFont* GetFont(std::string FontName, int Height);
};

#endif // GLFontManager_H_
