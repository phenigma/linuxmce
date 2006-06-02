#ifndef OPENGLGRAPHIC_H_
#define OPENGLGRAPHIC_H_

#include<string>

#include "TextureManager.h"

#include <SDL.h>

class OpenGLGraphic
{
public:
	OpenGLTexture Texture;
	float MaxU, MaxV;
	int Width, Height;

	OpenGLGraphic();
	virtual ~OpenGLGraphic();
	
	bool SetupFromImage(std::string FileName);
	
	virtual void Convert(SDL_Surface* Surface);
    
	virtual void ReleaseTexture();
};

#endif /*OPENGLGRAPHIC_H_*/
