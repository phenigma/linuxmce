#ifndef GLFontManager_H_
#define GLFontManager_H_

#include "GLFont.h"

class GLFontManager
{
	std::map <std::string, GLFont*> Fonts;

	std::string FontToString(std::string FontName, int Height);

	static GLFontManager* Instance_;
public:

	static GLFontManager* GetInstance();

	void CleanUp();

	bool Exists(std::string FontStr);
	GLFont* GetFont(std::string FontName, int Height);
};

#endif // GLFontManager_H_
