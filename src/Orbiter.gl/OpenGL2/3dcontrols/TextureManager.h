#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

typedef GLuint OpenGLTexture; 

class TextureManager
{
	
public:
	TextureManager(void);
	virtual ~TextureManager(void);
	
	static void SetupTexture(OpenGLTexture Texture);
	
	
};
