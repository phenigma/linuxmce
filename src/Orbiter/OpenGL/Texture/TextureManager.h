#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

#include <queue>

typedef GLuint OpenGLTexture; 

class OpenGLGraphic;

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* _Instance;
	
	std::queue <OpenGLGraphic*> WaitForConvert;
public:
	static TextureManager* Instance();

	virtual ~TextureManager(void);
	
	void SetupTexture(OpenGLTexture Texture);
	
	void PrepareImage(OpenGLGraphic* TextureGraphic);
	void ConvertImagesToTextures();
		
};
