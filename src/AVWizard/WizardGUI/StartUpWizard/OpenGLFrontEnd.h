#ifndef OpenGLFrontEnd_H_
#define OpenGLFrontEnd_H_

#include "GenericBackEnd.h"

class OpenGLFrontEnd : public GenericBackEnd
{
	SDL_Surface* Display;
public:
	OpenGLFrontEnd(void);
	virtual ~OpenGLFrontEnd(void);

	int StartVideoMode(int Width, int Height, bool FullScreen);
	virtual void Flip(int LeftBorder, int TopBorder, int Border);

	virtual int TextOutput(char* Text, int Left, int Top, TColorDesc Color, int Mode);

};

#endif
