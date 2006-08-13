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
	return;
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