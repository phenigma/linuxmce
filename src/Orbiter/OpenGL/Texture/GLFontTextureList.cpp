#include "GLFontTextureList.h"

#include <SDL_ttf.h>
#include <iostream>

#include <GL/gl.h>

#include "../Mesh/MeshPainter.h"
#include "../GLMathUtils.h"

//remove me
#include "DCE/Logger.h"
#include "../../SDL/PlutoSDLDefs.h"

GLFontTextureList::GLFontTextureList(TTF_Font* Font, 
									 int Style, int FontHeight,
									 unsigned char R, 
									 unsigned char G,
									 unsigned char B) 
	: Font_(Font),
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

	OpenGLGraphic* LetterGraphic = new OpenGLGraphic();
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
	LetterGraphic->LocalSurface = RenderedText;
	LetterGraphic->Width = RenderedText->w;
	LetterGraphic->Height = RenderedText->h;
	
	TextureManager::Instance()->PrepareConvert(LetterGraphic);
	Letters[Letter] = LetterGraphic;
}

/*virtual*/ int GLFontTextureList::TextOut(int X, int Y, std::string Text, MeshContainer* &Geometry)
{
	int StartX = X;
	size_t Length = Text.length();
	if (Length == 0)
		return NULL;
	int LetterLen = 0, FontHeight;
	unsigned char CharPos = 32;
	
	float MaxU, MaxV;
	
	MeshBuilder MB;
  	MeshContainer* Container;
  	MeshTransform Transform;
	
	MB.Begin(MBMODE_TRIANGLES);
	MB.SetColor(1.0f, 1.0f, 1.0f);

	FontHeight = FontHeight_;

	int PixelHgt = FontHeight;
	MB.SetBlended(true);

	for(size_t i = 0; i<Length; i++)
	{
		CharPos = (unsigned char)Text[i];
		MapLetter(CharPos);
		
		MaxU = Letters[CharPos]->MaxU;
		MaxV = Letters[CharPos]->MaxV;

		LetterLen = int(Letters[CharPos]->Width * MaxU);

		int PixelLen = LetterLen;

		MB.SetTexture(Letters[CharPos]);
		// Point 1
		MB.SetTexture2D(0.0f, 0.0f);
		MB.AddVertexFloat(float(X), float(Y), 0);

		// Point 2
		MB.SetTexture2D(1.0f, 0.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y), 0);

		// Point 3
		MB.SetTexture2D(0.0f, 1.0f);
		MB.AddVertexFloat(float(X), float(Y+PixelHgt), 0);

		// Point 3
		MB.SetTexture2D(0.0f, 1.0f);
		MB.AddVertexFloat(float(X), float(Y+PixelHgt), 0);

		// Point 2
		MB.SetTexture2D(1.0f, 0.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y), 0);

		// Point 4
		MB.SetTexture2D(1.0f, 1.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y+PixelHgt), 0);

		X+= PixelLen;
		
		//DCE::g_pPlutoLogger->Write(LV_STATUS, "TextOut: %f %f %f \n", (float)X, (float)Y, (float)FontHeight);
	}

	Container = MB.End();
	
	//Painter->PaintContainer(*Container, Transform);

	Geometry = Container;

	return X - StartX;
}

bool GLFontTextureList::Exists(unsigned char Letter)
{
	return (Letters.find(Letter) != Letters.end());
}
