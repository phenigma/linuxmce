#include "GLFontTextureList.h"

#include <SDL_ttf.h>
#include <iostream>

#include <GL/gl.h>

#include "../Mesh/MeshPainter.h"

GLFontTextureList::GLFontTextureList()
{

}

GLFontTextureList::~GLFontTextureList()
{
}

/*virtual*/ void GLFontTextureList::MapFont(
		std::string FontName,
		int Height, 
		int Style
		)
{
	TTF_Font *Font = TTF_OpenFont(FontName.c_str(), Height);
	if(Font == NULL)
	{
		return;
	}
	TTF_SetFontStyle(Font, Style);
	char Text[2];
	Text[1] = 0;
	
	for(int i = 1; i<256; i++)
	{
		SDL_Color SDL_color;

		SDL_color.r = 255;
		SDL_color.g = 255;
		SDL_color.b = 255;
		SDL_color.unused = 255;
	
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
	
		if (RenderedText == NULL)
			return;
			
		Letters[i].Prepare(RenderedText);
			
		SDL_FreeSurface(RenderedText);
				
	}
		
	
	TTF_CloseFont(Font);
}

/*virtual*/ MeshContainer* GLFontTextureList::TextOut(int X, int Y, char* Text)
{
	int Length = strlen(Text);
	int LetterLen = 0, FontHeight;
	unsigned char CharPos = 0;
	float MaxU, MaxV;
	OpenGLTexture Texture;
	
	MeshBuilder MB;
  	MeshContainer* Container;
  	MeshTransform Transform;
	
/*

	MB.SetTexture(Texture.Texture);
	
	MB.SetTexture2D(0.0f, 0.0f);
	MB.AddVertexFloat(-64, -48, 44);
	MB.SetTexture2D(Texture.MaxU, 0.0f);
	MB.AddVertexFloat( 64, -48, 44);
	MB.SetTexture2D(0.0f, Texture.MaxV);
	MB.AddVertexFloat( -64, 48, 44);
	MB.SetTexture2D(Texture.MaxU, Texture.MaxV);
	MB.AddVertexFloat(64, 48, 44);
	
*/

	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetColor(1.0f, 1.0f, 1.0f);
	
	FontHeight = Letters[CharPos].Height;
	float PixelMaxV = Letters[CharPos].MaxV;
	int PixelHgt = FontHeight*2;


	for(int i = 0; i<Length; i++)
	{
		CharPos = (unsigned char)Text[i];
 		LetterLen = Letters[CharPos].Width;
 		MaxU = Letters[CharPos].MaxU;
 		MaxV = Letters[CharPos].MaxV;

		Texture = Letters[CharPos].Texture;
		glBindTexture(GL_TEXTURE_2D, Texture);

		int PixelLen = LetterLen*2;
		float PixelMaxU = Letters[CharPos].MaxU;

		MB.SetTexture(Texture);
		// Point 1
		MB.SetTexture2D(0.0f, 0.0f);
		MB.AddVertexFloat(X, Y, 480);

		// Point 2
		MB.SetTexture2D(0.0f, PixelMaxV);
		MB.AddVertexFloat(X, Y+PixelHgt, 480);

		// Point 3
		MB.SetTexture2D(PixelMaxU, 0.0f);
		MB.AddVertexFloat(X+PixelLen, Y, 480);

		// Point 4
		MB.SetTexture2D(PixelMaxU, PixelMaxV);
		MB.AddVertexFloat(X+PixelLen, Y+PixelHgt, 480);

		X+= PixelLen;
		
		//printf("%f %f %f \n", (float)X, (float)Y, (float)FontHeight);
	}

	Container = MB.End();
	
	//Painter->PaintContainer(*Container, Transform);

	return Container;
}
