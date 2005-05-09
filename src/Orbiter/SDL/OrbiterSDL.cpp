/*
 OrbiterSDL

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "OrbiterSDL.h"
#include "SerializeClass/ShapesColors.h"

#include "PlutoSDLDefs.h"
#include "../RendererOCG.h"
#include "SDLRendererOCGHelper.h"

#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"
#include "SDLGraphic.h"

#include "SDL_rotozoom.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#ifndef BLUETOOTH_DONGLE
#define USE_ONLY_SCREEN_SURFACE
#endif

#ifdef WINCE
	#include "wince.h"
#endif

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterSDL::OrbiterSDL(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode,
					   int nImageWidth, int nImageHeight, bool bFullScreen/*=false*/)
    : Orbiter(DeviceID, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight)
{
    m_pScreenImage = NULL;
    m_nImageWidth=nImageWidth;
    m_nImageHeight=nImageHeight;
	m_bFullScreen=bFullScreen;

	Uint32 uSDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

	if(m_bFullScreen)
		uSDLInitFlags |= SDL_FULLSCREEN;

	if (SDL_Init(uSDLInitFlags) == -1)
    {
#ifndef WINCE
		cerr << "Failed to initialize SDL" << SDL_GetError() << endl;
#else
		printf("Failed to initialize SDL %s\n", SDL_GetError());
#endif //WINCE

		exit(1);
    }

	atexit(SDL_Quit);
    g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

#ifndef BLUETOOTH_DONGLE
	Uint32 uVideoModeFlags = SDL_SWSURFACE;

#if !defined(WIN32) || defined(WINCE)
	if(m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
#endif

	if ((Screen = SDL_SetVideoMode(m_nImageWidth, m_nImageHeight, 0, uVideoModeFlags)) == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Failed to set video mode: %s", SDL_GetError());
        exit(1);
    }
#endif

    g_pPlutoLogger->Write(LV_STATUS, "Set video mode to %d x %d Window.", m_nImageWidth, m_nImageHeight);

#ifdef USE_ONLY_SCREEN_SURFACE
	m_pScreenImage = Screen;
#else
    m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, m_nImageWidth, m_nImageHeight, 32, rmask, gmask, bmask, amask);
    if (m_pScreenImage == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
    }
#endif
	m_bWeCanRepeat = true;

    g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterSDL::~OrbiterSDL()
{
	KillMaintThread(); // We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
g_pPlutoLogger->Write(LV_STATUS, "about to free surface");

#ifndef USE_ONLY_SCREEN_SURFACE
	SDL_FreeSurface(m_pScreenImage);
#endif

    m_pScreenImage = NULL;

g_pPlutoLogger->Write(LV_STATUS, "~OrbiterSDL finished");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderScreen()
{
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() : " NO SCREEN"));

    if (m_pScreenHistory_Current)
    {
        PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
    }

    Orbiter::RenderScreen();
    DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);

#ifndef USE_ONLY_SCREEN_SURFACE
    SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
#endif

	SDL_UpdateRect(Screen, 0, 0, 0, 0);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RedrawObjects()
{
    //PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
    Orbiter::RedrawObjects();
}

//-----------------------------------------------------------------------------------------------------
void WrapAndRenderText(void *Surface, string text, int X, int Y, int W, int H,
                       string FontPath, TextStyle *pTextStyle);

#include "pluto_main/Define_Text.h"
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderText(DesignObjText *Text,TextStyle *pTextStyle)
{
    string TextToDisplay = SubstituteVariables(Text->m_sText, NULL, 0, 0).c_str();
    SDL_Rect TextLocation;
    TextLocation.x = Text->m_rPosition.X;
    TextLocation.y = Text->m_rPosition.Y;
    TextLocation.w = Text->m_rPosition.Width;
    TextLocation.h = Text->m_rPosition.Height;

if( Text->m_PK_Text==TEXT_USR_ENTRY_CONST )
{
g_pPlutoLogger->Write(LV_STATUS,"*******rendering timeout at %d,%d - %d,%d",TextLocation.x,TextLocation.y,TextLocation.w,TextLocation.h );
}

#ifdef WIN32
    string BasePath="C:\\Windows\\Fonts\\";
#else
    string BasePath="/usr/pluto/fonts/";
#endif //win32

	try
	{
		WrapAndRenderText(m_pScreenImage, TextToDisplay, TextLocation.x, TextLocation.y, TextLocation.w, TextLocation.h, BasePath, pTextStyle);
	}
	catch(...)
	{
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{
	sge_Rect(m_pScreenImage,X,Y,Width + X,Height + Y,color.m_Value);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity /*= 100*/)
{
    SDL_Rect Rectangle;
    Rectangle.x = x; Rectangle.y = y; Rectangle.w = width; Rectangle.h = height;

	SDL_FillRect(m_pScreenImage, &Rectangle, SDL_MapRGBA(m_pScreenImage->format, color.R(), color.G(), color.B(), color.A()));
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderGraphic(PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal,
	bool bDisableAspectRatio)
{
	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtSDLGraphic)
		return;//nothing to render or not an sdl graphic

	if(pPlutoGraphic->IsEmpty())
		return;

	SDLGraphic *pSDLGraphic = (SDLGraphic *) pPlutoGraphic;
	SDL_Surface *pSDL_Surface = pSDLGraphic->m_pSDL_Surface;

	//render the sdl surface
	SDL_Rect Destination;
	Destination.x = rectTotal.X; Destination.y = rectTotal.Y;
	Destination.w = rectTotal.Width; Destination.h = rectTotal.Height;

	if(pSDL_Surface->w == 0 || pSDL_Surface->h == 0) //nothing to render
		return; //malformated image?
	else
		if(pSDL_Surface->w != rectTotal.Width || pSDL_Surface->h != rectTotal.Height) //different size. we'll have to stretch the surface
		{
			double ZoomX = 1;
			double ZoomY = 1;

			SDL_Surface *rotozoom_picture;

			if(bDisableAspectRatio) //no aspect ratio kept
			{
				ZoomX = rectTotal.Width / double(pSDL_Surface->w);
				ZoomY = rectTotal.Height / double(pSDL_Surface->h);
			}
			else //we'll have to keep the aspect
			{
				ZoomX = ZoomY = min(rectTotal.Width / double(pSDL_Surface->w),
					rectTotal.Height / double(pSDL_Surface->h));
			}

			rotozoom_picture = zoomSurface(pSDL_Surface, ZoomX, ZoomY, SMOOTHING_ON);

			SDL_BlitSurface(rotozoom_picture, NULL, m_pScreenImage, &Destination);
			SDL_FreeSurface(rotozoom_picture);
		}
		else //same size ... just blit the surface
			SDL_BlitSurface(pSDL_Surface, NULL, m_pScreenImage, &Destination);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj)
{

    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
		pObj->m_rPosition.Width, pObj->m_rPosition.Height, 32, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = pObj->m_rPosition.Left(); SourceRect.y = pObj->m_rPosition.Top();
	SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;

	SDL_SetAlpha(m_pScreenImage, 0, 0);
	SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);

    pObj->m_pGraphicToUndoSelect = new SDLGraphic(pSDL_Surface);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL::putpixel(SDL_Surface * pSDL_Surface, int x, int y, Uint32 pixel_color)
{
    // don't try to put a pixel outside the pSDL_Surface
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return;

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        * pixel = pixel_color;
        break;

    case 2:
        * (Uint16 *) pixel = pixel_color;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            pixel[0] = (pixel_color >> 16) & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = pixel_color & 0xff;
        }
        else
        {
            pixel[0] = pixel_color & 0xff;
            pixel[1] = (pixel_color >> 8) & 0xff;
            pixel[2] = (pixel_color >> 16) & 0xff;
        }
        break;

    case 4:
        * (Uint32 *) pixel = pixel_color;
        break;
    }
}
//-----------------------------------------------------------------------------------------------------
Uint32 OrbiterSDL::getpixel(SDL_Surface * pSDL_Surface, int x, int y)
{
    // all pixels outside the pSDL_Surface are black
    if (x < 0 || x >= pSDL_Surface->w || y < 0 || y >= pSDL_Surface->h)
        return SDL_MapRGB(pSDL_Surface->format, 0, 0, 0);

    int bpp = pSDL_Surface->format->BytesPerPixel;
    Uint8 * pixel = (Uint8 *) pSDL_Surface->pixels + y * pSDL_Surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
        return * pixel;

    case 2:
        return * (Uint16 *) pixel;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
        else
            return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

    case 4:
        return * (Uint32 *) pixel;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
	SDL_PixelFormat * PF = m_pScreenImage->format;
	Uint32 PlutoPixelDest, PlutoPixelSrc, Pixel;

	g_pPlutoLogger->Write(LV_STATUS, "ReplaceColor: %u %u %u : %u %u %u",
		ColorToReplace.R(), ColorToReplace.G(), ColorToReplace.B(),
		ReplacementColor.R(), ReplacementColor.G(), ReplacementColor.B());

	PlutoPixelSrc = (ColorToReplace.R() << PF->Rshift) | (ColorToReplace.G() << PF->Gshift) | (ColorToReplace.B() << PF->Bshift) | (ColorToReplace.A() << PF->Ashift);
	unsigned char *Source = (unsigned char *) &PlutoPixelSrc;
	PlutoPixelDest = ReplacementColor.R() << PF->Rshift | ReplacementColor.G() << PF->Gshift | ReplacementColor.B() << PF->Bshift;//  TODO -- this should work | ReplacementColor.A() << PF->Ashift;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // we may need locking on the surface
			Pixel = getpixel(m_pScreenImage, i + x, j + y);
			unsigned char *pPixel = (unsigned char *) &Pixel;
			const int max_diff = 20;
			if ( abs(Source[0]-pPixel[0])<max_diff && abs(Source[1]-pPixel[1])<max_diff && abs(Source[2]-pPixel[2])<max_diff && abs(Source[3]-pPixel[3])<max_diff )
			{
				putpixel(m_pScreenImage, i + x, j + y, PlutoPixelDest);
			}

        }
    }

	PlutoRectangle rect(x, y, width, height);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::BeginPaint()
{
	//g_pPlutoLogger->Write(LV_STATUS, "Begin paint.");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::EndPaint()
{
	//if we are using a buffer surface to blit images and text (Bluetooth_Dongle uses this)
	//will have to update the hole screen
	//if not, the user will call UpdateRect function for each rectangle he must invalidate
#ifndef USE_ONLY_SCREEN_SURFACE
	DisplayImageOnScreen(m_pScreenImage);
#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::OnQuit()
{
	g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
	m_bQuit = true;
	SDL_Event *pEvent = new SDL_Event;
	pEvent->type = SDL_QUIT;
	SDL_PushEvent(pEvent);
	delete pEvent;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *OrbiterSDL::CreateGraphic()
{
	return new SDLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::UpdateRect(PlutoRectangle rect)
{
	//clipping the rectangle
	//	SDL_UpdateRect dies in Linux if we are trying to update
	//	a rectangle outside the screen
	if(rect.X < 0)
		rect.X = 0;

	if(rect.Y < 0)
		rect.Y = 0;

	if(rect.Right() >= m_Width)
		rect.Width = m_Width - rect.X - 1;

	if(rect.Bottom() >= m_Height)
		rect.Height = m_Height - rect.Y - 1;

	PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);

#ifdef USE_ONLY_SCREEN_SURFACE
	SDL_UpdateRect(Screen, rect.Left(), rect.Top(), rect.Width, rect.Height);
#endif
}
//-----------------------------------------------------------------------------------------------------
