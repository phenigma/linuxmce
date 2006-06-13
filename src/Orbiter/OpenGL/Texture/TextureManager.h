#ifndef TextureManager_H_
#define TextureManager_H_

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
	bool TextureEnable_;
	
	std::queue <OpenGLGraphic*> WaitForConvert;
public:
	static TextureManager* Instance();

	virtual ~TextureManager(void);
	
	void SetupTexture(OpenGLTexture Texture);
	
	void PrepareImage(OpenGLGraphic* TextureGraphic);
	void ConvertImagesToTextures();
		
};

#endif //TextureManager_H_
