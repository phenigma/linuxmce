#ifndef RENDERER_H
#define RENDERER_H

// these should be split into multiple headers?

#include <iostream>
#include <string>
using namespace std;

#include "Orbiter/DesignObj_Data.h"
#include "SDL.h"

//------------------------------------------------------------------------

class TextObj
{
	public:
		enum AlignFlags {Left = 0, Center = 1, Right = 2, Top = 4, Middle = 8, Bottom = 16};

		PlutoPoint position;
		string text;
		bool bold, italic, underline;
		AlignFlags Align;

		TextObj() : position(0, 0), text(""), bold(false), italic(false), underline(false) {}
};

//------------------------------------------------------------------------

class RendererImage
{
	public:
		string m_sFilename;
		SDL_Surface * m_pSDL_Surface;
		bool NewSurface;
		
		RendererImage() : m_sFilename(""), m_pSDL_Surface(NULL), NewSurface(true) {}
		~RendererImage() { if (m_pSDL_Surface) SDL_FreeSurface(m_pSDL_Surface); }
};

//------------------------------------------------------------------------

// TODO: write API docs for this one
class Renderer
{
public:
	string m_sFontPath,m_sOutputDirectory;
	int m_Width,m_Height;
	SDL_Surface * Screen;

	Renderer(string FontPath,string OutputDirectory,int Width,int Height);
	~Renderer();

	RendererImage * CreateBlankCanvas(PlutoSize size = PlutoSize(0,0));

	void RenderObject(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint Position,int iRenderStandard,bool bPreserveAspectRatio);

	void RenderObjectsChildren(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,bool bPreserveAspectRatio);
	void RenderObjectsText(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,int iIteration);

	void SaveImageToFile(RendererImage * pRendererImage, string sSaveToFile);
	
	RendererImage *Subset(RendererImage *pRenderImage,PlutoRectangle rect);
	// If Crop is true and PreserveAspectRatio is true, then instead of shrinking to fit within the given space, it will
	// fill the target space, and any excess will be cropped
	RendererImage *CreateFromFile(string sFilename, PlutoSize size=PlutoSize(0,0),bool bPreserveAspectRatio=true,bool bCrop=false);
	void CompositeImage(RendererImage *pRenderImage_Parent,RendererImage *pRenderImage_Child,PlutoPoint pos);
	void RenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator *pDesignObj_Generator, PlutoPoint pos);
	PlutoSize RealRenderText(RendererImage * pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator * pDesignObj_Generator, PlutoPoint pos);

protected:
	Uint32 getpixel(RendererImage * RIsurface, int x, int y);
	void putpixel(RendererImage * RIsurface, int x, int y, Uint32 pixel_color);
};

//------------------------------------------------------------------------

#endif
