#include "GLFontTextureList.h"

#include <SDL_ttf.h>
#include <iostream>

#include <GL/gl.h>

#include "../Mesh/MeshPainter.h"


//remove me
#include "DCE/Logger.h"
#include "../../SDL/PlutoSDLDefs.h"

GLFontTextureList::GLFontTextureList(int ScreenHeight, TTF_Font* Font, 
									 int Style, int FontHeight,
									 unsigned char R, 
									 unsigned char G,
									 unsigned char B) 
	: ScreenHeight_(ScreenHeight),
	  Font_(Font),
	  Style_(Style),
	  FontHeight_(FontHeight)
{
	FontColor.r = R;
	FontColor.g = G;
	FontColor.b = B;
	FontColor.unused = 255; //opaque
}

GLFontTextureList::~GLFontTextureList()
{
	std::map<char, OpenGLGraphic*> ::iterator Item;
	for(Item = Letters.begin(); Item != Letters.end(); Item++)
	{
		delete Item->second;
	}
	Letters.clear();
}

/*virtual*/ void GLFontTextureList::MapLetter(char Letter)
{
	if(Exists(Letter))
		return;

	Letters[Letter] = new OpenGLGraphic();
	char Text[2];
	Text[0] = Letter;
	Text[1] = 0;
	
	SDL_Surface * RenderedText = NULL;
	TTF_SetFontStyle(Font_, Style_);

	try
	{
		RenderedText = TTF_RenderText_Blended(Font_, Text, FontColor);
	}
	catch(...) //if the clipping rectagle is too big, SDL_FreeSurface will crash
	{
		std::cout<<"Renderer::RealRenderText : TTF_RenderText_Blended crashed!"<<std::endl;
	}
	
	Letters[Letter]->LocalSurface = RenderedText;
	Letters[Letter]->Width = RenderedText->w;
	Letters[Letter]->Height = RenderedText->h;
	TextureManager::Instance()->PrepareImage(Letters[Letter]);
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
		MapLetter(CharPos);
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

bool GLFontTextureList::Exists(unsigned char Letter)
{
	return (Letters.find(Letter) != Letters.end());
}
