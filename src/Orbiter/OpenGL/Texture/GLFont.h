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

	int ScreenHeight_;
	int Height_;
public:
	GLFont(int ScreenHeight);
	~GLFont(void);

	bool OpenFont(std::string FontName,	int Height);

	void CloseFont();

	GLFontTextureList* GetFontStyle(int Style, int R, int G, int B);


};

#endif
