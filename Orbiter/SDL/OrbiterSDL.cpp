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

#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"

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

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
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

		throw "Failed to initialize SDL";
    }

	atexit(SDL_Quit);
    g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

#ifndef BLUETOOTH_DONGLE
	Uint32 uVideoModeFlags = SDL_SWSURFACE;

	if(m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
	
	if ((Screen = SDL_SetVideoMode(800, 600, 0, uVideoModeFlags)) == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Failed to set video mode: %s", SDL_GetError());
        throw "Failed to set video mode";
    }
#endif

    g_pPlutoLogger->Write(LV_STATUS, "Set video mode to 800x600 Window.");

#ifdef USE_ONLY_SCREEN_SURFACE
	m_pScreenImage = Screen;
#else
    m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, m_nImageWidth, m_nImageHeight, 32, rmask, gmask, bmask, amask);
    if (m_pScreenImage == NULL)
    {
        g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
    }
#endif

    g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterSDL::~OrbiterSDL()
{
#ifdef USE_ONLY_SCREEN_SURFACE
	SDL_FreeSurface(m_pScreenImage);
#endif

    m_pScreenImage = NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderScreen()
{
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$");
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
    if (m_pScreenHistory_Current)
    {
        SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
    }

    Orbiter::RenderScreen();
    DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);

g_pPlutoLogger->Write(LV_STATUS,"Enter display image on screen");

#ifndef USE_ONLY_SCREEN_SURFACE
    SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
#endif

	SDL_UpdateRect(Screen, 0, 0, 0, 0);

	g_pPlutoLogger->Write(LV_STATUS,"Exit display image on screen");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RedrawObjects()
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
    Orbiter::RedrawObjects();
}

//-----------------------------------------------------------------------------------------------------
void WrapAndRenderText(void *Surface, string text, int X, int Y, int W, int H,
                       string FontPath, TextStyle *pTextStyle);
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderText(DesignObjText *Text,TextStyle *pTextStyle)
{
    string TextToDisplay = SubstituteVariables(Text->m_sText, NULL, 0, 0).c_str();
    SDL_Rect TextLocation;
    TextLocation.x = Text->m_rPosition.X;
    TextLocation.y = Text->m_rPosition.Y;
    TextLocation.w = Text->m_rPosition.Width;
    TextLocation.h = Text->m_rPosition.Height;

#ifdef WIN32
    string BasePath="C:\\Windows\\Fonts\\";
#else
    string BasePath="/usr/pluto/fonts/";
#endif //win32

	WrapAndRenderText(m_pScreenImage, TextToDisplay, TextLocation.x, TextLocation.y, TextLocation.w, TextLocation.h, BasePath, pTextStyle);
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
/*virtual*/ void OrbiterSDL::RenderGraphic(DesignObj_Orbiter *pObj, PlutoRectangle rectTotal, bool bDisableAspectRatio)
{
    bool bDeleteSurface=true;  // Will set to false if we're going to cache
    SDL_Surface * obj_image = NULL;
    if( pObj->m_pCurrentGraphic->GraphicType_get()==gtSDLGraphic )
    {
        SDLGraphic *pSDL = (SDLGraphic *) pObj->m_pCurrentGraphic;
        obj_image = pSDL->m_pSDL_Surface;
    }
    else if( pObj->m_pCurrentGraphic->GraphicType_get()==gtIMGraphic )
    {
        IMGraphic *pIMGraphic = (IMGraphic *) pObj->m_pCurrentGraphic;
        obj_image = pIMGraphic->m_pSDL_Surface;
        if( !obj_image && m_sLocalDirectory.length()>0 )
        {
            obj_image = IMG_Load ((m_sLocalDirectory + pIMGraphic->m_Filename).c_str());
            if( !obj_image )
            {
                g_pPlutoLogger->Write(LV_CRITICAL, "Unable to read file %s",(m_sLocalDirectory + pIMGraphic->m_Filename).c_str());
                return;
            }
        }
        else if( !obj_image )
        {
            // Request our config info
            char *pGraphicFile=NULL;
            int iSizeGraphicFile=0;

            DCE::CMD_Request_File CMD_Request_File(m_dwPK_Device,m_dwPK_Device_GeneralInfoPlugIn,"orbiter/C" + StringUtils::itos(m_dwPK_Device) + "/" + pIMGraphic->m_Filename,
                &pGraphicFile,&iSizeGraphicFile);
            SendCommand(CMD_Request_File);

            if (!iSizeGraphicFile)
            {
                g_pPlutoLogger->Write(LV_CRITICAL, "Unable to get file from server %s", pIMGraphic->m_Filename.c_str());
                return;
            }

            SDL_RWops * rw = SDL_RWFromMem(pGraphicFile, iSizeGraphicFile);
            obj_image = IMG_Load_RW(rw, 1); // rw is freed here
            delete pGraphicFile;

        }
#ifdef CACHE_IMAGES
        pIMGraphic->m_pSDL_Surface = obj_image;  // In case we loaded from cache
        bDeleteSurface = false;
#endif
    }
    else if( pObj->m_pCurrentGraphic->GraphicType_get()==gtWinGraphic )
    {
        WinGraphic *pWinGraphic = (WinGraphic *) pObj->m_pCurrentGraphic;
        if( pWinGraphic->m_pCompressedImage && pWinGraphic->m_CompressedImageLength )
        {
            SDL_RWops * rw = SDL_RWFromMem(pWinGraphic->m_pCompressedImage, pWinGraphic->m_CompressedImageLength);
            obj_image = IMG_Load_RW(rw, 1); // rw is freed here
        }
    }
    else
        g_pPlutoLogger->Write(LV_CRITICAL,"SDL Got a type of graphic I don't know how to render");

    if( !obj_image )
        return;

	SDL_Rect Destination;
    Destination.x = rectTotal.X; Destination.y = rectTotal.Y;
	Destination.w = pObj->m_rPosition.Width; Destination.h = pObj->m_rPosition.Height;
    
	SDL_BlitSurface(obj_image, NULL, m_pScreenImage, &Destination);

	if( bDeleteSurface )
        SDL_FreeSurface(obj_image);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj)
{

    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
		pObj->m_rPosition.Width, pObj->m_rPosition.Height, 32, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = pObj->m_rPosition.Left(); SourceRect.y = pObj->m_rPosition.Top();
	SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;

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
void OrbiterSDL::Initialize(GraphicType Type)
{
    Orbiter::Initialize(Type);
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
	PlutoPixelDest = ReplacementColor.R() << PF->Rshift | ReplacementColor.G() << PF->Gshift | ReplacementColor.B() << PF->Bshift | ReplacementColor.A() << PF->Ashift;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // we may need locking on the surface
			Pixel = getpixel(m_pScreenImage, i + x, j + y);
			if (Pixel == PlutoPixelSrc)
			{
				putpixel(m_pScreenImage, i + x, j + y, PlutoPixelDest);
			}
        }
    }
}
//-----------------------------------------------------------------------------------------------------	
/*virtual*/ void OrbiterSDL::BeginPaint()
{	
	g_pPlutoLogger->Write(LV_STATUS, "Begin paint.");
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::EndPaint()
{
	g_pPlutoLogger->Write(LV_STATUS, "End paint.");
	DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
