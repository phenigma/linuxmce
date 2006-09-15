/*
Orbiter

Copyright (C) 2006 Pluto, Inc., a Florida Corporation

www.plutohome.com

Phone: +1 (877) 758-8648

This program is distributed according to the terms of the Pluto Public License, available at:
http://plutohome.com/index.php?section=public_license

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

@author: "Ciprian Mustiata" ciprian.m at plutohome dot com, "Cristian Miron" chris.m at plutohome dot com 

*/
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
		//RenderedText = TTF_RenderText_Blended(Font_, Text, FontColor);
		
		RenderedText = TTF_RenderUTF8_Blended(Font_, Text, FontColor);
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

/*virtual*/ int GLFontTextureList::TextOut(int Width, std::string Text, MeshContainer* &Geometry)
{
	int X = 0;
	int Y = 0;

	Geometry = NULL;

	int StartX = X;
	size_t Length = Text.length();
	if (Length == 0)
		return 0;

	int LetterLen = 0, FontHeight;
	unsigned char CharPos = 32;
	
	float MaxU, MaxV;
	
	MeshBuilder MB;
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

		if(Width < X + PixelLen - StartX)
		{
			Geometry = MB.End();
			return X - StartX;
		}


		MB.SetTexture(Letters[CharPos]);
		// Point 1
		MB.SetTexture2D(0.0f, 0.0f);
		MB.AddVertexFloat(float(X), float(Y), 0);

		// Point 3
		MB.SetTexture2D(0.0f, 1.0f);
		MB.AddVertexFloat(float(X), float(Y+PixelHgt), 0);

		// Point 2
		MB.SetTexture2D(1.0f, 0.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y), 0);

		// Point 2
		MB.SetTexture2D(1.0f, 0.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y), 0);

		// Point 3
		MB.SetTexture2D(0.0f, 1.0f);
		MB.AddVertexFloat(float(X), float(Y+PixelHgt), 0);

		// Point 4
		MB.SetTexture2D(1.0f, 1.0f);
		MB.AddVertexFloat(float(X+PixelLen), float(Y+PixelHgt), 0);

		X+= PixelLen;

		//DCE::g_pPlutoLogger->Write(LV_STATUS, "TextOut: %f %f %f \n", (float)X, (float)Y, (float)FontHeight);
	}

	Geometry = MB.End();

	return X - StartX;
}

bool GLFontTextureList::Exists(unsigned char Letter)
{
	return (Letters.find(Letter) != Letters.end());
}
