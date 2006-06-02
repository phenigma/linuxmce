#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>
#include <SDL.h>

class OpenGLGraphic;
typedef GLuint OpenGLTexture; 

class TextureManager
{
	OpenGLTexture LastTexture;
	
	TextureManager();
	static TextureManager* _Instance;
public:
	OpenGLGraphic* OldScreen;
	OpenGLGraphic* NewScreen;

	static TextureManager* Instance();

	virtual ~TextureManager(void);
	
	void SetupTexture(OpenGLTexture Texture);
	void ConfigureNextScreen(SDL_Surface* NextScreen);
};
