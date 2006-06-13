#include "GLFontTextureList.h"

#include <SDL_ttf.h>
#include <iostream>

#include <GL/gl.h>

#include "../Mesh/MeshPainter.h"


//remove me
#include "DCE/Logger.h"
#include "../../SDL/PlutoSDLDefs.h"

GLFontTextureList::GLFontTextureList(int ScreenHeight) : ScreenHeight_(ScreenHeight)
{
	for(int i = 0; i < 256; i++)
		Letters[i] = new OpenGLGraphic();
}

GLFontTextureList::~GLFontTextureList()
{
	for(int i = 0; i < 256; i++)
		delete Letters[i];
}

/*virtual*/ void GLFontTextureList::MapFont(
		std::string FontName,
		int Height, 
		int Style,
		unsigned char R, 
		unsigned char G,
		unsigned char B
		)
{
#ifdef WIN32
#ifdef WINCE
	string BasePath = "C:\\Windows\\Fonts\\";
#else
	char pWindowsDirector[MAX_PATH];
	GetWindowsDirectory(pWindowsDirector, MAX_PATH);
	string BasePath = string(pWindowsDirector) + "\\Fonts\\";
#endif

#else
	string BasePath="/usr/share/fonts/truetype/msttcorefonts/";
#endif //win32	

	string sPathFont = BasePath + FontName + ".ttf";

	TTF_Font *Font = TTF_OpenFont(sPathFont.c_str(), Height);
	if(Font == NULL)
	{
		return;
	}
	TTF_SetFontStyle(Font, Style);
	char Text[2];
	Text[1] = 0;
	
	for(int i = 1; i < 256; i++)
	{
		SDL_Color SDL_color;

		SDL_color.r = R;
		SDL_color.g = G;
		SDL_color.b = B;
		SDL_color.unused = 255; //opaque
	
		SDL_Surface * RenderedText = NULL;
		Text[0] = i;

		try
		{
			RenderedText = TTF_RenderText_Blended(Font, Text, SDL_color);
		}
		catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
		{
			std::cout<<"Renderer::RealRenderText : TTF_RenderText_Blended crashed!"<<std::endl;
		}
		
		Letters[i]->LocalSurface = RenderedText;
		Letters[i]->Width = RenderedText->w;
		Letters[i]->Height = RenderedText->h;
		TextureManager::Instance()->PrepareImage(Letters[i]);
	}
		
	
	TTF_CloseFont(Font);
}

/*virtual*/ MeshContainer* GLFontTextureList::TextOut(int X, int Y, char* Text)
{
	int Length = int(strlen(Text));
	if (Length == 0)
		return NULL;
	int LetterLen = 0, FontHeight;
	unsigned char CharPos = 32;
	float MaxU, MaxV;
	
	MeshBuilder MB;
  	MeshContainer* Container;
  	MeshTransform Transform;
	
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);
	
	FontHeight = Letters[CharPos]->Height;
	float PixelMaxV = Letters[CharPos]->MaxV;

	int PixelHgt = FontHeight;
	MB.SetBlended(true);

	for(int i = 0; i<Length; i++)
	{
		CharPos = (unsigned char)Text[i];
		MaxU = Letters[CharPos]->MaxU;
		MaxV = Letters[CharPos]->MaxV;

		LetterLen = int(Letters[CharPos]->Width * MaxU);

		int PixelLen = LetterLen;
		float PixelMaxU = Letters[CharPos]->MaxU;

		MB.SetTexture(Letters[CharPos]);
		// Point 1
		MB.SetTexture2D(0.0f, 0.0f);
		MB.AddVertexFloat(float(X), float(Y), ScreenHeight_ / 2.0f);

		// Point 2
		MB.SetTexture2D(0.0f, PixelMaxV);
		MB.AddVertexFloat(float(X), float(Y+PixelHgt), ScreenHeight_ / 2.0f);

		// Point 3
		MB.SetTexture2D(PixelMaxU, 0.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y), ScreenHeight_ / 2.0f);

		// Point 4
		MB.SetTexture2D(PixelMaxU, PixelMaxV);
		MB.AddVertexFloat(float(X+PixelLen), float(Y+PixelHgt), ScreenHeight_ / 2.0f);

		X+= PixelLen;
		
		//DCE::g_pPlutoLogger->Write(LV_STATUS, "TextOut: %f %f %f \n", (float)X, (float)Y, (float)FontHeight);
	}

	Container = MB.End();
	
	//Painter->PaintContainer(*Container, Transform);

	return Container;
}
