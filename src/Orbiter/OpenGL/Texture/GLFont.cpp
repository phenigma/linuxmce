#include "GLFont.h"

#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#endif

#include "PlutoUtils/StringUtils.h"


GLFont::GLFont(int ScreenHeight)
: ScreenHeight_(ScreenHeight)
{
}

GLFont::~GLFont(void)
{
	if(Font)
	{
		TTF_CloseFont(Font);
		Font = NULL;
	}  
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
	Styles[StrStyle] = new GLFontTextureList(ScreenHeight_, Font, Style, Height_, R, G, B);

	return Styles[StrStyle];
}
