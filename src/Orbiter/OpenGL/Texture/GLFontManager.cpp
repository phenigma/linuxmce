#include "GLFontManager.h"

#include "PlutoUtils/StringUtils.h"

/*static*/ GLFontManager* GLFontManager::Instance_ = NULL;

/*static*/ GLFontManager* GLFontManager::GetInstance()
{
	if(Instance_ == NULL)
		Instance_ = new GLFontManager();
	return Instance_;
}

void GLFontManager::Setup(int ScreenHeight)
{
	ScreenHeight_ = ScreenHeight;
}

void GLFontManager::CleanUp()
{

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
	GLFont* Result = new GLFont(ScreenHeight_); 
	Result->OpenFont(FontName, Height);
	Fonts[FontStr] = Result;
	return Result;
}