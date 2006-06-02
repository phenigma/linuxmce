#ifndef __OPENGL_GRAPHIC_H__
#define __OPENGL_GRAPHIC_H__
//-------------------------------------------------------------------------------------------------------
#include "../DesignObj_Orbiter.h"
#include "SDL.h"
#include "GLDefs.h"
#include <GL/gl.h>


//-------------------------------------------------------------------------------------------------------
class OpenGLGraphic : public PlutoGraphic
{
public:
	OpenGLGraphic(string Filename, eGraphicManagement GraphicManagement, Orbiter *pCI);
	OpenGLGraphic(struct SDL_Surface *pSDL_Surface);
	OpenGLGraphic(Orbiter *pCI);
	~OpenGLGraphic();

	void Initialize();
	GraphicType GraphicType_get() { return gtOpenGLGraphic; }
	bool IsEmpty() { return NULL == m_pSDL_Surface; }
	bool LoadGraphic(char *pData, size_t iSize,int iRotation=0);
	void Clear(); 
	virtual PlutoGraphic *GetHighlightedVersion();
	PlutoGraphic* Clone();
	bool GetInMemoryBitmap(char*& pRawBitmapData, size_t& ulSize);

	struct SDL_Surface *m_pSDL_Surface;
	OpenGLTexture m_OpenGLTexture;

	//helper methods
	static Uint32 getpixel(struct SDL_Surface *pSDL_Surface,int x, int y);
	static void putpixel(struct SDL_Surface *pSDL_Surface,int x, int y, Uint32 pixel_color);
};
//-------------------------------------------------------------------------------------------------------
#endif //__OPENGL_GRAPHIC_H__

