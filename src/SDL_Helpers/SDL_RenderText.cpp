/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include "SDL_RenderText.h"
#include "SDL_RenderUtils.h"

#include "SDL_ttf.h"
//---------------------------------------------------------------------------------------------------
PlutoSize SDL_RenderText::RealRenderText(RendererImage *pRenderImage, DesignObjText *pDesignObjText, 
						 TextStyle *pTextStyle, PlutoPoint pos)
{
	int nPixelHeight = pTextStyle->m_iPixelHeight; 
	if(NULL == pTextStyle->m_pTTF_Font)
	{
		if (pTextStyle->m_sFont == "")
			pTextStyle->m_sFont = "arial";

		string sFontFile = m_sFontPath + pTextStyle->m_sFont + ".ttf";
		pTextStyle->m_pTTF_Font = TTF_OpenFont(sFontFile.c_str(), nPixelHeight);
		if(NULL == pTextStyle->m_pTTF_Font)
		{
			string sUpperCasedFontFile = StringUtils::ToUpper(m_sFontPath + pTextStyle->m_sFont + ".ttf");
			pTextStyle->m_pTTF_Font = TTF_OpenFont(sUpperCasedFontFile.c_str(), nPixelHeight);
			if(NULL == pTextStyle->m_pTTF_Font)
			{
				string sLowerCasedFontFile = StringUtils::ToLower(m_sFontPath + pTextStyle->m_sFont + ".ttf");
				pTextStyle->m_pTTF_Font = TTF_OpenFont(sLowerCasedFontFile.c_str(), nPixelHeight);
				if(NULL == pTextStyle->m_pTTF_Font)
				{
					TTF_Quit();
					return PlutoSize(1, 1);
				}
			}
		}
	}

	SDL_Color SDL_color;
	SDL_color.r=pTextStyle->m_ForeColor.R();
	SDL_color.g=pTextStyle->m_ForeColor.G();
	SDL_color.b=pTextStyle->m_ForeColor.B();
	SDL_color.unused = pTextStyle->m_ForeColor.A();

	/* Underline combined with anything crashes */
	int style = TTF_STYLE_NORMAL;
	if (pTextStyle->m_bUnderline)
	{
		style = TTF_STYLE_UNDERLINE;
	}
	else
	{
		if (pTextStyle->m_bBold)
			style |= TTF_STYLE_BOLD;
		if (pTextStyle->m_bItalic)
			style |= TTF_STYLE_ITALIC;
	}

	TTF_SetFontStyle((TTF_Font *) pTextStyle->m_pTTF_Font, style);
	SDL_Surface * RenderedText = NULL;

	//BOM (Byte Order Mask) header for UTF-8 strings
	const char BOM_header[3] = { (char)0xef, (char)0xbb, (char)0xbf };

	//UTF-8 string?
	bool bHasBOMHeader = !memcmp(BOM_header, pDesignObjText->m_sText.data(), sizeof(BOM_header));

	if(bHasBOMHeader)
	{
		//render utf-8 text
		RenderedText = TTF_RenderUTF8_Blended((TTF_Font *)pTextStyle->m_pTTF_Font, 
			pDesignObjText->m_sText.c_str(), SDL_color);
	}
	else
	{
		RenderedText = TTF_RenderText_Blended((TTF_Font *)pTextStyle->m_pTTF_Font, 
			pDesignObjText->m_sText.c_str(), SDL_color);
	}

	if (RenderedText == NULL)
	{
		TTF_Quit();
		return PlutoSize(1, 1);
	}

	PlutoSize RenderedSize(RenderedText->w, RenderedText->h);
	if(pRenderImage != NULL)
	{
		pRenderImage->NewSurface = false;
		RendererImage * pRI_RenderedText = new RendererImage();
		pRI_RenderedText->m_pSurface = RenderedText;
		CompositeImage(pRenderImage, pRI_RenderedText, pos);

		pRI_RenderedText->m_pSurface = NULL;
		delete pRI_RenderedText;
	}

	SDL_FreeSurface(RenderedText);
	return RenderedSize;
}
//---------------------------------------------------------------------------------------------------
pair<int, int> SDL_RenderText::GetWordWidth(string Word, TextStyle *pTextStyle, 
											RendererImage *& RI, bool NewSurface/* = true*/)
{
	// Radu was creating the render for every call
	RendererImage * Canvas;

	DesignObjText DOText;
	DOText.m_sText = Word;

	PlutoSize Size = RealRenderText(NULL, &DOText, pTextStyle, PlutoPoint(0,0));
	if (!NewSurface)
	{
		Canvas = (RendererImage *) RI;
		delete Canvas;
	}

	Canvas = CreateBlankCanvas(Size);
	RealRenderText(Canvas, &DOText, pTextStyle, PlutoPoint(0,0));

	RI = Canvas;
	return pair<int, int>(Size.Width, Size.Height);
}
//---------------------------------------------------------------------------------------------------
