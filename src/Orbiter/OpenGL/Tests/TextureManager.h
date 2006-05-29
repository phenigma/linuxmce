#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

typedef GLuint OpenGLTexture; 

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* _Instance;
public:
	static TextureManager* Instance();

	virtual ~TextureManager(void);
	
	void SetupTexture(OpenGLTexture Texture);
	
	
};
