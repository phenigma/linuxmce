#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <string>
using namespace std;
// these should be split into multiple headers?

#include "Orbiter/DesignObj_Data.h"
#include "SDL.h"
#include "Orbiter/RendererImage.h"
#include "Orbiter/RendererMNG.h"

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

// TODO: write API docs for this one
class Renderer
{
public:
	string m_sFontPath,m_sOutputDirectory;
	int m_Width,m_Height;
	SDL_Surface * Screen;

	Renderer(string FontPath,string OutputDirectory,int Width,int Height,bool bDisableVideo=false);
	~Renderer();

	static RendererImage * CreateBlankCanvas(PlutoSize size);
	PlutoSize RealRenderText(RendererImage * pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, PlutoPoint pos);

#ifndef ORBITER
	void RenderObject(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,
	PlutoPoint Position,int iRenderStandard,bool bPreserveAspectRatio,int iOnlyVersion=-999);
	void RenderObjectsChildren(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,bool bPreserveAspectRatio,int iOnlyVersion);
	void RenderObjectsText(RendererImage *pRenderImage,DesignObj_Generator *pDesignObj_Generator,PlutoPoint pos,int iIteration);
	static void SaveImageToFile(RendererImage * pRendererImage, string sSaveToFile);
	static void SaveImageToPNGFile(RendererImage * pRendererImage, FILE * File, bool Signature = true);
	static RendererImage *Subset(RendererImage *pRenderImage,PlutoRectangle rect);
	// If Crop is true and PreserveAspectRatio is true, then instead of shrinking to fit within the given space, it will
	// fill the target space, and any excess will be cropped
	static RendererImage *CreateFromFile(string sFilename, PlutoSize size=PlutoSize(0,0),bool bPreserveAspectRatio=true,bool bCrop=false);
	static RendererImage *CreateFromFile(FILE * File, PlutoSize size=PlutoSize(0,0),bool bPreserveAspectRatio=true,bool bCrop=false);
	static RendererImage *CreateFromRWops(SDL_RWops * rw, bool bFreeRWops = true, PlutoSize size=PlutoSize(0,0),bool bPreserveAspectRatio=true,bool bCrop=false);
	static void CompositeImage(RendererImage *pRenderImage_Parent,RendererImage *pRenderImage_Child,PlutoPoint pos);
	static RendererImage * DuplicateImage(RendererImage * pRendererImage);
	void RenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, TextStyle *pTextStyle, DesignObj_Generator *pDesignObj_Generator, PlutoPoint pos);

	static RendererMNG * CreateMNGFromFile(string FileName, PlutoSize Size);
	static RendererMNG * CreateMNGFromFiles(const vector<string> & FileNames, PlutoSize Size);
	static void SaveMNGToFile(string FileName, RendererMNG * MNG);
#endif

protected:
	static Uint32 getpixel(RendererImage * RIsurface, int x, int y);
	static void putpixel(RendererImage * RIsurface, int x, int y, Uint32 pixel_color);
};

//------------------------------------------------------------------------

#endif
