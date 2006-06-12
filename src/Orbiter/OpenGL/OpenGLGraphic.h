#ifndef OPENGLGRAPHIC_H_
#define OPENGLGRAPHIC_H_

#include<string>

#include "Texture/TextureManager.h"
#include "../PlutoGraphic.h"

#include <SDL.h>

class OpenGLGraphic : public PlutoGraphic
{
	

public:


	//move me
SDL_Surface* LocalSurface;

	OpenGLTexture Texture;
	float MaxU, MaxV;
	int Width, Height;

	OpenGLGraphic();
	OpenGLGraphic(OrbiterRenderer *pOrbiterRenderer);
	OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, OrbiterRenderer *pOrbiterRenderer);
	~OpenGLGraphic();

	bool SetupFromImage(std::string FileName);
	void Prepare(SDL_Surface* Surface);
	void Convert();

	//from PlutoGraphic
	PlutoGraphic* Clone(); 
	void Initialize();
	GraphicType GraphicType_get();
	bool IsEmpty(); 
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0); 
	void Clear(); 
	PlutoGraphic *GetHighlightedVersion() { return NULL; }
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);
};

#endif /*OPENGLGRAPHIC_H_*/
