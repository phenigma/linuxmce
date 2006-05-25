#ifndef __OpenGLTextureConverter_SDL_H__
#define __OpenGLTextureConverter_SDL_H__

#include "SDL.h"

#include "../OpenGL/OpenGLTextureConverter.h"
#include "SDL.h"

class OpenGLTextureConverter_SDL : public OpenGLTextureConverter {
public:
	OpenGLTextureConverter_SDL(PlutoGraphic *pGraphic);
	
	virtual ~OpenGLTextureConverter_SDL() {}
	OpenGLTexture Convert();
	
private:
	SDL_Surface* CreateSurfaceTexture(SDL_Surface* Surface);
};

#endif //__OpenGLTextureConverter_SDL_H__
