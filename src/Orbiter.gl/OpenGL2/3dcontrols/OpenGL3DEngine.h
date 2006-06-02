#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include <SDL.h>

#include "ExtensionManager.h"

class OpenGL3DEngine
{
	SDL_Surface* Screen;
public:
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual bool InitVideoMode(int Width, int Height, int Bpp, bool FullScreen);
	virtual void Finalize(void);
};

#endif
