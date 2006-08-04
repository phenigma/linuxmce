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

class OpenGLGraphic;
class MeshFrame;
class OpenGL3DEngine;

class TextureManager
{
	OpenGLTexture LastTexture;
	TextureManager();
	static TextureManager* Instance_;
	bool TextureEnable_;

	int SupportTextureNonPowerOfTwo_;
	
	std::queue <OpenGLGraphic*> WaitForConvert;
	std::queue <OpenGLGraphic*> WaitForRelease;
	std::map<std::string, MeshFrame*> Graphics;

	bool ExistInCache(std::string ObjectID);
	OpenGL3DEngine *Engine;

public:
	static TextureManager* Instance();
	virtual ~TextureManager(void);
	

	void Setup(OpenGL3DEngine *Engine);

	void SetupTexture(OpenGLTexture Texture);
	void CleanUp();
	
	void PrepareConvert(OpenGLGraphic* TextureGraphic);
	void ConvertImagesToTextures();
	void PrepareRelease(OpenGLGraphic* TextureGraphic);
	void ReleaseTextures();

	bool SupportTextureNonPowerOfTwo();

	MeshFrame* GetCacheItem(std::string ObjectID);
	void AddCacheItem(std::string ObjectID, MeshFrame* Frame);

	void AttachToScene(std::string ObjectID, MeshFrame* Frame);
};

#endif //TextureManager_H_
