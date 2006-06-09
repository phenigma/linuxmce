/*
 OrbiterRenderer_SDL

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "OrbiterRenderer_SDL.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "DCE/Logger.h"
#include "SerializeClass/ShapesColors.h"

#include "PlutoSDLDefs.h"
#include "../RendererOCG.h"
#include "SDLRendererOCGHelper.h"

#include "pluto_main/Define_Effect.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DataGrid.h"
#include "SDLGraphic.h"
#include "Splitter/TextWrapper.h"
#include "../ScreenHistory.h"
#include "../Orbiter.h"

#ifdef WIN32
#include "../Win32/MainDialog.h"
#endif

#include "SDL_rotozoom.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <sge.h>
#include <sge_surface.h>

#if !defined(BLUETOOTH_DONGLE) && !defined(PROXY_ORBITER)
#define USE_ONLY_SCREEN_SURFACE
#endif

#ifdef WINCE
    #include "wince.h"
#endif

using namespace DCE;

bool g_bResettingVideoMode;
void *SetVideoModeWatchDogThread(void *p)
{
    OrbiterRenderer_SDL *pOrbiterRenderer_SDL = (OrbiterRenderer_SDL *) p;
    g_pPlutoLogger->Write(LV_STATUS,"Inside WatchDogThread");
    Sleep(2000);
    if( g_bResettingVideoMode )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "SDL_SetVideoMode hangs on init. Xorg is not initialized yet? Restarting...");
#ifndef WIN32
        kill(getpid(), SIGKILL);
#endif
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL::OrbiterRenderer_SDL(Orbiter *pOrbiter) : OrbiterRenderer(pOrbiter)
{
	m_bRelativeMode = false;
	m_pScreenImage = NULL;
}

OrbiterRenderer_SDL::~OrbiterRenderer_SDL()
{
	// We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
	g_pPlutoLogger->Write(LV_STATUS, "about to free surface");

	#ifndef USE_ONLY_SCREEN_SURFACE
		SDL_FreeSurface(m_pScreenImage);
	#endif

	m_pScreenImage = NULL;
	g_pPlutoLogger->Write(LV_STATUS, "~OrbiterRenderer_SDL finished");
}

void OrbiterRenderer_SDL::Configure()
{
	/// SDL 2D code, creates the SDL window
	//initializing the engine...
	Uint32 uSDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

	if(m_bFullScreen)
		uSDLInitFlags |= SDL_FULLSCREEN;

	if (SDL_Init(uSDLInitFlags) == -1)
	{
	#ifndef WINCE
			cerr << "Failed initializing SDL: " << SDL_GetError() << endl;
	#else
			printf("Failed to initialize SDL %s\n", SDL_GetError());
	#endif //WINCE

	#ifndef WIN32 //linux
            string sCmd = "/usr/pluto/bin/Start_X.sh";//; /usr/pluto/bin/Start_WM.sh";
            g_pPlutoLogger->Write(LV_CRITICAL, "X is not running! Starting X and the window manager: %s", sCmd.c_str());
            system(sCmd.c_str());
	#endif //linux
	
		exit(1);
	}

	SDL_WM_SetCaption("OrbiterRenderer_SDL", "OrbiterRenderer_SDL");

	atexit(SDL_Quit);
	g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

#ifdef USE_ONLY_SCREEN_SURFACE
	Uint32 uVideoModeFlags = SDL_SWSURFACE | SDL_RESIZABLE;

#if !defined(WIN32) || defined(WINCE)
	if(m_bFullScreen)
		uVideoModeFlags |= SDL_FULLSCREEN;
#endif

	g_bResettingVideoMode=true;
	pthread_t hackthread;
	pthread_create(&hackthread, NULL, SetVideoModeWatchDogThread, (void*)this);

    Screen = SDL_SetVideoMode(OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 0, uVideoModeFlags);
    if (Screen == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Failed to set video mode (%d x %d): %s", OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, SDL_GetError());
		exit(1);
	}
	g_bResettingVideoMode=false;
    InitializeAfterSetVideoMode();
#endif

	g_pPlutoLogger->Write(LV_STATUS, "Set video mode to %d x %d Window.", OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);

#ifdef USE_ONLY_SCREEN_SURFACE
	m_pScreenImage = Screen;
#else
    m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 32, rmask, gmask, bmask, amask);
	if (m_pScreenImage == NULL)
	{
		g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
	}
#endif
	OrbiterLogic()->m_bWeCanRepeat = true;

	g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderScreen( bool bRenderGraphicsOnly )
{
	#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",
			(OrbiterLogic()->m_pScreenHistory_Current ? OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
	#endif
	
	if (OrbiterLogic()->m_pScreenHistory_Current)
	{
		PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
        LockDisplay();
        SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
        UnlockDisplay();
    }

	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);
	DisplayImageOnScreen(m_pScreenImage);
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
	if(OrbiterLogic()->m_bQuit)
		return;

	PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);
    LockDisplay();

#ifndef USE_ONLY_SCREEN_SURFACE
    SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
#endif

	SDL_UpdateRect(Screen, 0, 0, 0, 0);
    UnlockDisplay();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderText(string &TextToDisplay,DesignObjText *Text,TextStyle *pTextStyle, PlutoPoint point)
{
    PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

	SDL_Rect TextLocation;
	TextLocation.x = point.X + Text->m_rPosition.X;
	TextLocation.y = point.Y + Text->m_rPosition.Y;
	TextLocation.w = Text->m_rPosition.Width;
	TextLocation.h = Text->m_rPosition.Height;

#ifdef WIN32
#ifdef WINCE
	string BasePath = "C:\\Windows\\Fonts";
#else
	char pWindowsDirector[MAX_PATH];
	GetWindowsDirectory(pWindowsDirector, MAX_PATH);
	string BasePath = string(pWindowsDirector) + "\\Fonts";
#endif

#else
	string BasePath="/usr/share/fonts/truetype/msttcorefonts";
#endif //win32

	WrapAndRenderText(m_pScreenImage, TextToDisplay, TextLocation.x, TextLocation.y, TextLocation.w, TextLocation.h, BasePath,
		pTextStyle,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment, &OrbiterLogic()->m_mapTextStyle);

}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{
	ClipRectangle(X, Y, Width, Height);
	sge_Rect(m_pScreenImage,X,Y,Width + X,Height + Y,color.m_Value);
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::SolidRectangle(int x, int y, int width, int height, PlutoColor color)
{
	ClipRectangle(x, y, width, height);

	SDL_Rect Rectangle;
	Rectangle.x = x; Rectangle.y = y; Rectangle.w = width; Rectangle.h = height;

	if(color.A() < 255)
	{
		SDL_Rect rectBar;
		rectBar.x = 0; rectBar.y = 0; rectBar.w = width; rectBar.h = height;
		SDL_Surface *pSDL_Bar = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, rectBar.w, rectBar.h, 32, rmask, gmask, bmask, amask);
		SDL_FillRect(pSDL_Bar, &rectBar, SDL_MapRGBA(pSDL_Bar->format, color.R(), color.G(), color.B(), color.A()));

		LockDisplay();
		SDL_BlitSurface(pSDL_Bar, NULL, m_pScreenImage, &Rectangle);
		UnlockDisplay();
		SDL_FreeSurface(pSDL_Bar);	
	}
	else
	{
		LockDisplay();
		SDL_FillRect(m_pScreenImage, &Rectangle, SDL_MapRGBA(m_pScreenImage->format, color.R(), color.G(), color.B(), color.A()));
		UnlockDisplay();
	}
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderGraphic(PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal,
    bool bDisableAspectRatio, PlutoPoint point)
{

	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtSDLGraphic)
		return;//nothing to render or not an sdl graphic

	if(pPlutoGraphic->IsEmpty())
		return;

    SDLGraphic *pSDLGraphic = (SDLGraphic *) pPlutoGraphic;
	SDL_Surface *pSDL_Surface = pSDLGraphic->m_pSDL_Surface;

	//render the sdl surface
	SDL_Rect Destination;
	Destination.x = point.X + rectTotal.X;
	Destination.y = point.Y + rectTotal.Y;
	Destination.w = rectTotal.Width;
	Destination.h = rectTotal.Height;

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

            LockDisplay();
			SDL_BlitSurface(rotozoom_picture, NULL, m_pScreenImage, &Destination);
			SDL_FreeSurface(rotozoom_picture);
            UnlockDisplay();
		}
		else //same size ... just blit the surface
        {
            LockDisplay();
            SDL_BlitSurface(pSDL_Surface, NULL, m_pScreenImage, &Destination);
            UnlockDisplay();
        }
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	SDL_Rect SourceRect;
	SourceRect.x = point.X + pObj->m_rPosition.Left(); SourceRect.y = point.Y + pObj->m_rPosition.Top();
	SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;
	
    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
            pObj->m_rPosition.Width, pObj->m_rPosition.Height, 32, rmask, gmask, bmask, amask);
	
    LockDisplay();
	SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
	SDL_SetAlpha(pSDL_Surface, SDL_RLEACCEL , SDL_ALPHA_OPAQUE);
    UnlockDisplay();
    
	pObj->m_pGraphicToUndoSelect = new SDLGraphic(pSDL_Surface);
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_SDL::GetBackground( PlutoRectangle &rect )
{
	ClipRectangle(rect);

    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
		rect.Width, rect.Height, 32, rmask, gmask, bmask, amask);

	SDL_Rect SourceRect;
	SourceRect.x = rect.Left(); SourceRect.y = rect.Top();
	SourceRect.w = rect.Width; SourceRect.h = rect.Height;

    LockDisplay();
	SDL_SetAlpha(m_pScreenImage, 0, 0);
	SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
    UnlockDisplay();
    
	return new SDLGraphic(pSDL_Surface);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
    SDL_PixelFormat * PF = m_pScreenImage->format;
    Uint32 PlutoPixelDest, PlutoPixelSrc, Pixel;

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "ReplaceColor: %u %u %u : %u %u %u",
        ColorToReplace.R(), ColorToReplace.G(), ColorToReplace.B(),
        ReplacementColor.R(), ReplacementColor.G(), ReplacementColor.B());
#endif

    PlutoPixelSrc = (ColorToReplace.R() << PF->Rshift) | (ColorToReplace.G() << PF->Gshift) | (ColorToReplace.B() << PF->Bshift) | (ColorToReplace.A() << PF->Ashift);
    unsigned char *Source = (unsigned char *) &PlutoPixelSrc;
    PlutoPixelDest = ReplacementColor.R() << PF->Rshift | ReplacementColor.G() << PF->Gshift | ReplacementColor.B() << PF->Bshift;//  TODO -- this should work | ReplacementColor.A() << PF->Ashift;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // we may need locking on the surface
            Pixel = SDLGraphic::getpixel(m_pScreenImage, i + x, j + y);
            unsigned char *pPixel = (unsigned char *) &Pixel;
            const int max_diff = 3;
            if ( abs(Source[0]-pPixel[0])<max_diff && abs(Source[1]-pPixel[1])<max_diff && abs(Source[2]-pPixel[2])<max_diff && abs(Source[3]-pPixel[3])<max_diff )
            {
                SDLGraphic::putpixel(m_pScreenImage,i + x, j + y, PlutoPixelDest);
            }

        }
    }

    PlutoRectangle rect(x, y, width, height);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::BeginPaint()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::EndPaint()
{
    //if we are using a buffer surface to blit images and text (Bluetooth_Dongle uses this)
    //will have to update the hole screen
    //if not, the user will call UpdateRect function for each rectangle he must invalidate
#ifndef USE_ONLY_SCREEN_SURFACE
    DisplayImageOnScreen(m_pScreenImage);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::OnQuit()
{
    g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
    SDL_Event *pEvent = new SDL_Event;
    pEvent->type = SDL_QUIT;
    SDL_PushEvent(pEvent);
    delete pEvent;
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_SDL::CreateGraphic()
{
    return new SDLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
    //clipping the rectangle
    PlutoRectangle localrect = rect;
    localrect.X += point.X;
    localrect.Y += point.Y;

	ClipRectangle(localrect);

#ifdef USE_ONLY_SCREEN_SURFACE
	PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

    LockDisplay();
    SDL_UpdateRect(Screen, localrect.Left(), localrect.Top(), localrect.Width, localrect.Height);
    UnlockDisplay();
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::OnIdle()
{
    Sleep(5);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::EventLoop()
{
	int SDL_Event_Pending = 0;

	SDL_Event Event;

   // For now I'll assume that shift + arrows scrolls a grid
    while (!OrbiterLogic()->m_bQuit && !OrbiterLogic()->m_bReload)
    {
		SDL_Event_Pending = SDL_PollEvent(&Event);

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			} 
			else if(Event.type == SDL_MOUSEMOTION)
			{
				if (m_bRelativeMode)
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_RELATIVE_MOVE;
					orbiterEvent.data.region.m_iX = Event.motion.xrel;
					orbiterEvent.data.region.m_iY = Event.motion.yrel;
				}
				else
				{
					orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
					orbiterEvent.data.region.m_iX = Event.button.x;
					orbiterEvent.data.region.m_iY = Event.button.y;
				}
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			} 
			else if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				OrbiterLogic()->ProcessEvent(orbiterEvent);

#if defined(WIN32) && !defined(PROXY_ORBITER) && !defined(BLUETOOTH_DONGLE)
				RecordMouseAction(Event.button.x, Event.button.y);
#endif
			}
			else if (Event.type == SDL_MOUSEBUTTONUP)
			{
				orbiterEvent.type = Orbiter::Event::REGION_UP;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			}
		}
		else
		{
			OnIdle();
		}
    }  // while
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::CaptureRelativeMovements() 
{ 
	m_bRelativeMode = true;

	SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_ShowCursor(SDL_DISABLE);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ReleaseRelativeMovements()
{ 
	m_bRelativeMode = false;

	SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_ShowCursor(SDL_ENABLE);
}
//-----------------------------------------------------------------------------------------------------
