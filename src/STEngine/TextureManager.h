#ifndef TextureManager_H_
#define TextureManager_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <WINDOWS.H>
#endif

#include <GL/gl.h>

#include <queue>
#include <map>
#include <string>
typedef GLuint OpenGLTexture; 

class GraphicImage;
class MeshFrame;
class OpenGL3DEngine;

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* Instance_;
	bool TextureEnable_;

	int SupportTextureNonPowerOfTwo_;

public:
	static TextureManager* Instance();
	virtual ~TextureManager(void);
	

	void SetupTexture(OpenGLTexture Texture);
	void CleanUp();

	void RemoveFromConvertQueue(GraphicImage* TextureGraphic);
 
	bool SupportTextureNonPowerOfTwo();
};

#endif //TextureManager_H_
