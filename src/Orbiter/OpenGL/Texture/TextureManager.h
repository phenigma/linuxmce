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
	static TextureManager* Instance_;
	bool TextureEnable_;

	int SupportTextureNonPowerOfTwo_;
	
	std::queue <OpenGLGraphic*> WaitForConvert;
	std::queue <OpenGLGraphic*> WaitForRelease;

	//helper methods
	bool CheckExtension(const GLubyte* extensions, const char* checkFor);

public:
	static TextureManager* Instance();
	virtual ~TextureManager(void);
	
	void SetupTexture(OpenGLTexture Texture);
	void CleanUp();
	
	void PrepareConvert(OpenGLGraphic* TextureGraphic);
	void ConvertImagesToTextures();
	void PrepareRelease(OpenGLGraphic* TextureGraphic);
	void ReleaseTextures();

	bool SupportTextureNonPowerOfTwo();
};

#endif //TextureManager_H_
