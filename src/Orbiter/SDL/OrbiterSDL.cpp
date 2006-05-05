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
#include "OrbiterSDL.h"

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
#include "Orbiter/ScreenHistory.h"

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

#if defined(WINCE) || defined(BLUETOOTH_DONGLE) || defined(PROXY_ORBITER)
#define DISABLE_OPENGL
#endif

#ifndef DISABLE_OPENGL
#include "../OpenGL/math3dutils.h"
#include "../OpenGL/GL2DWidgets/basicwindow.h"
#include "../OpenGL/GL2DWidgets/DrawingWidgetsEngine.h"

#include "../OpenGL/orbitergl3dengine.h"

#include "../OpenGL/GL2DEffects/gl2deffecttransit.h"
#include "../OpenGL/GL2DEffects/gl2deffectbeziertranzit.h"
#include "../OpenGL/GL2DEffects/gl2deffectslidefromleft.h"
#include "../OpenGL/GL2DEffects/gl2deffectfadesfromtop.h"
#include "../OpenGL/GL2DEffects/gl2deffectfadesfromunderneath.h"
#include "../OpenGL/Orbiter3DCommons.h" 
#include "../Simulator.h" 

#include "../../pluto_main/Define_Effect.h"
#endif

bool g_bResettingVideoMode;
void *HackThread2(void *p)
{
    OrbiterSDL *pOrbiterSDL = (OrbiterSDL *) p;
    // For some reason X can fail to properly die????  TODO - HACK
    g_pPlutoLogger->Write(LV_STATUS,"Inside Hacktrhead #2");
    Sleep(2000);
    if( g_bResettingVideoMode )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Another stupid problem since the kernel upgrade.  SDL_SetVideoMode can just hang on init.  It happened. Kill ourselves");
#ifndef WIN32
        kill(getpid(), SIGSEGV);
#endif
    }
    return NULL;
}

#ifndef DISABLE_OPENGL

/**
 * OpenGL drawing thread. It is in three steps: 
 * - Init OpenGL with SDL
 * - looping to drawing frames
 * - end the OpenGL
 */


void *Orbiter_OpenGLThread(void *p)
{
	OrbiterSDL * pOrbiter = (OrbiterSDL*) p;
	// Create the OpenGL window and classes asociated with it
	// OpenGL drawing operations, locked in the mutex
	PLUTO_SAFETY_LOCK(cm, *(pOrbiter->m_GLThreadMutex));

	pOrbiter->m_Desktop = OrbiterGL3D::GetInstance();
	pOrbiter->m_Desktop->BuildOrbiterGL(pOrbiter);
	cm.CondWait(); // This will unlock the mutex and lock it on awakening
	
	pOrbiter->PaintDesktopGL = false;

	while(!pOrbiter->m_bQuit)
	{
		//nothing to process. let's sleep...
		cm.CondWait(); // This will unlock the mutex and lock it on awakening

		/// Thread is waken up, for more reasons as Quit event or there is one effect pending or 
		/// is idle event
		// Orbiter is in Quit state, end the thread
		if(pOrbiter->m_bQuit)
			return NULL;

		if (!pOrbiter->m_Desktop->EffectBuilder->HasEffects()) {
			if (!pOrbiter->PaintDesktopGL) {
				pOrbiter->PaintDesktopGL = true;
				pOrbiter->m_Desktop->Paint();
			}
			else
			{
				cm.Release();
				Sleep(10);
				cm.Relock();
			}
		}
		else 
		{
			pOrbiter->PaintDesktopGL = false;
			pOrbiter->m_Desktop->Paint();
		}
	}

	delete pOrbiter->m_Desktop;

	return NULL;
}	
#endif

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterSDL::OrbiterSDL(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,
    bool bLocalMode, int nImageWidth, int nImageHeight, bool bFullScreen/*=false*/,
    pluto_pthread_mutex_t *pExternalScreenMutex/*=NULL*/, 
	bool UseOpenGL /*=false*/)
 : Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth,
    nImageHeight, pExternalScreenMutex)
{
	// Use OpenGL if is requested
	EnableOpenGL = UseOpenGL;

#ifdef DISABLE_OPENGL
	EnableOpenGL = false;
#endif

	m_spPendingGLEffects.reset(new PendingGLEffects());

	m_pScreenImage = NULL;
	m_bFullScreen=bFullScreen;
	if(EnableOpenGL)
	{
#ifndef DISABLE_OPENGL
		/// if OpenGL is used there is created one OpenGL drawing mutex
		m_GLThreadMutex = new pluto_pthread_mutex_t("open gl worker thread");
		SDLGLthread = 0;

		pthread_cond_init(&m_GLThreadCond, NULL);
		m_GLThreadMutex->Init(NULL, &m_GLThreadCond);
		m_Desktop = NULL;
#endif
	}
}

/*virtual*/ OrbiterSDL::~OrbiterSDL()
{
	KillMaintThread(); 
	// We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
	g_pPlutoLogger->Write(LV_STATUS, "about to free surface");

	#ifndef USE_ONLY_SCREEN_SURFACE
		SDL_FreeSurface(m_pScreenImage);
	#endif

	m_pScreenImage = NULL;

	g_pPlutoLogger->Write(LV_STATUS, "~OrbiterSDL finished");

	if(EnableOpenGL)
	{
#ifndef DISABLE_OPENGL
		/// Wait OpenGL to finish
		pthread_cond_broadcast(&m_GLThreadCond);

		if(SDLGLthread)
			pthread_join(SDLGLthread, NULL);

		delete m_GLThreadMutex;
#endif
	}
}

/*virtual*/ bool OrbiterSDL::GetConfig()
{
	if(!Orbiter::GetConfig())
		return false;
	
	if (!EnableOpenGL)
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
	
        // TODO: move in StartOrbiterSDL.cpp : init_system()
		#ifndef WIN32 //linux
                string sCmd = "/usr/pluto/bin/Start_X.sh";//; /usr/pluto/bin/Start_WM.sh";
                g_pPlutoLogger->Write(LV_CRITICAL, "X is not running! Starting X and the window manager: %s", sCmd.c_str());
                system(sCmd.c_str());
		#endif //linux
		
			exit(1);
		}
	
		SDL_WM_SetCaption("OrbiterSDL", "OrbiterSDL");
	
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
		pthread_create(&hackthread, NULL, HackThread2, (void*)this);

        X_LockDisplay();
        Screen = SDL_SetVideoMode(m_iImageWidth, m_iImageHeight, 0, uVideoModeFlags);
        X_UnlockDisplay();
        if (Screen == NULL)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Failed to set video mode (%d x %d): %s", m_iImageWidth, m_iImageHeight, SDL_GetError());
			exit(1);
		}
		g_bResettingVideoMode=false;
	#endif
	
		g_pPlutoLogger->Write(LV_STATUS, "Set video mode to %d x %d Window.", m_iImageWidth, m_iImageHeight);
	
	#ifdef USE_ONLY_SCREEN_SURFACE
		m_pScreenImage = Screen;
	#else
        m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, m_iImageWidth, m_iImageHeight, 32, rmask, gmask, bmask, amask);
		if (m_pScreenImage == NULL)
		{
			g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
		}
	#endif
		m_bWeCanRepeat = true;
	
		g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");
	} //if (!EnableOpenGL)
	else
	{
#ifndef DISABLE_OPENGL
		/// if OpenGL is used there is created one OpenGL drawing thread 
		pthread_create(&SDLGLthread, NULL, Orbiter_OpenGLThread, (void*)this);
		/// creates 2D surface which is drawed with 2D engine the Orbiter
		/// which lately will be used as a texture
        m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, m_iImageWidth, m_iImageHeight, 32,
            rmask, gmask, bmask, amask);
		if (m_pScreenImage == NULL) {
			g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
		}
#endif
	}
    return true;
}
//----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, 
	DesignObj_Orbiter *pObj, PlutoPoint *ptPopup/* = NULL*/)
{
	if(pObj->m_PK_Effect_On_Screen > 0)
		m_spPendingGLEffects->m_nOnScreenTransitionEffectID = pObj->m_PK_Effect_On_Screen;

	Orbiter::ObjectOnScreen(pVectDesignObj_Orbiter, pObj, ptPopup);
}
//----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::ObjectOffScreen(DesignObj_Orbiter *pObj)
{
	if(pObj->m_PK_Effect_Off_Screen > 0)
        m_spPendingGLEffects->m_nOffScreenTransitionEffectID = pObj->m_PK_Effect_Off_Screen;

	Orbiter::ObjectOffScreen(pObj);
}
//----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderScreen()
{
	#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
	#endif
	
	if (!EnableOpenGL)
	{
		if (m_pScreenHistory_Current)
		{
			PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
            X_LockDisplay();
            SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
            X_UnlockDisplay();
        }
	
		Orbiter::RenderScreen();
		DisplayImageOnScreen(m_pScreenImage);
	}
	else //EnableOpenGL
	{
#ifndef DISABLE_OPENGL
		PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
		PLUTO_SAFETY_LOCK(glm, *m_GLThreadMutex);
		PlutoRectangle rectLastSelected(0, 0, 0, 0);
	
		//saving the surface before rendering the screen
        X_LockDisplay();
        SDL_Surface *pBeforeRender = SDL_CreateRGBSurface(SDL_HWSURFACE, m_iImageWidth, m_iImageHeight, 32,
             rmask, gmask, bmask, amask);
        X_UnlockDisplay();
			
		SDL_Rect ImageSize;
		ImageSize.x = 0;
		ImageSize.y = 0;
		ImageSize.w = m_iImageWidth;
		ImageSize.h = m_iImageHeight;
        X_LockDisplay();
        SDL_BlitSurface(m_pScreenImage, NULL, pBeforeRender, NULL);
        X_UnlockDisplay();
	
		m_spBeforeGraphic.reset(new SDLGraphic(pBeforeRender));
		if (m_pScreenHistory_Current)
        {
            X_LockDisplay();
            SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
            X_UnlockDisplay();
        }
	
		//screen rendering
		Orbiter::RenderScreen();
	
		//the surface after the screen was rendered
		if(m_spAfterGraphic.get())
			m_spAfterGraphic->Initialize();
	
		m_spAfterGraphic.reset(new SDLGraphic(m_pScreenImage));
		
		if(m_pObj_SelectedLastScreen)
		{
			rectLastSelected = PlutoRectangle(m_pObj_SelectedLastScreen->m_rPosition);
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "No object selected? :o");
			rectLastSelected.X = 0;
			rectLastSelected.Y = 0;
			rectLastSelected.Width = 80;
			rectLastSelected.Height = 80;
		}
	
		g_pPlutoLogger->Write(LV_CRITICAL, "Last selected object rectangle : x %d, y %d, w %d, h %d",
			rectLastSelected.X, rectLastSelected.Y, rectLastSelected.Width, rectLastSelected.Height);

		if(m_pObj_SelectedLastScreen)
			m_spPendingGLEffects->m_nOnSelectWithChangeEffectID = m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange;


		//TODO: Ciprian
		//You have here:
		int a = m_spPendingGLEffects->m_nOffScreenTransitionEffectID;
		int b = m_spPendingGLEffects->m_nOnScreenTransitionEffectID;
		int c = m_spPendingGLEffects->m_nOnSelectWithChangeEffectID;
		
		if(m_pObj_SelectedLastScreen)
		{
			//m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange = rand() % 9 + 1;

			//Create transtition effect if there is no effect already pending
			if(m_Desktop && m_Desktop->EffectBuilder)
			{
				m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
						m_Desktop->EffectBuilder->GetEffectCode(m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange),
						Simulator::GetInstance()->m_iMilisecondsTransition
					);
				if(!Transit)
					Transit = m_Desktop->EffectBuilder->
					CreateEffect(
						GL2D_EFFECT_TRANSIT_NO_EFFECT,
						Simulator::GetInstance()->m_iMilisecondsTransition
					);
				if(Transit)
					Transit->Configure(&rectLastSelected);
			} 
		}
		else
		{
			if(!UsesUIVersion2())
			{
				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
					GL2D_EFFECT_TRANSIT_NO_EFFECT,
					400
					);
			}
			else
			{
				//TODO: this is temporary
				int nCurrentDesignObjID = m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
				PlutoRectangle rectStartEffect(0, 0, m_iImageWidth, m_iImageHeight);
				int nPK_Effect = 0;
				int nTransitionTimeIsMs = Simulator::GetInstance()->m_iMilisecondsTransition; 
				switch(nCurrentDesignObjID) 
				{
				case DESIGNOBJ_mnuMenu2_CONST:
					nPK_Effect = EFFECT_Basic_transit_effect_CONST;
					break;

				case DESIGNOBJ_mnuAmbiance_CONST:
					nTransitionTimeIsMs = 1000;
					rectStartEffect.Location(PlutoPoint(0, m_iImageHeight - 40));
					rectStartEffect.Size(PlutoSize(100, 40));
					nPK_Effect = EFFECT_Bezier_transit_prism_CONST;
					break;

				case DESIGNOBJ_mnuDvdSpeedControl_CONST:
					nTransitionTimeIsMs = 1000;
					rectStartEffect.Location(PlutoPoint(0, m_iImageHeight - 40));
					rectStartEffect.Size(PlutoSize(100, 40));
					nPK_Effect = EFFECT_Bezier_transit_flow_slide_left_CONST;
					break;

				case DESIGNOBJ_popFileList_CONST:
					nPK_Effect = EFFECT_Fades_from_top_CONST;
					break;

				default:
					break;
				}

				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
					m_Desktop->EffectBuilder->GetEffectCode(nPK_Effect),
					nTransitionTimeIsMs
					);

				if(Transit)
					Transit->Configure(&rectStartEffect);
			}
		}
	
		glm.Release();
#endif
    }

	m_spPendingGLEffects->Reset();
}

/*virtual*/ void OrbiterSDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
	if(m_bQuit)
		return;

	if(!EnableOpenGL)
	{
		PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
        X_LockDisplay();
	
	#ifndef USE_ONLY_SCREEN_SURFACE
        SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
	#endif
	
		SDL_UpdateRect(Screen, 0, 0, 0, 0);
        X_UnlockDisplay();
	}
	else
	{
#ifndef DISABLE_OPENGL
		WakeupFromCondWait();
#endif
	}

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RedrawObjects()
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
    Orbiter::RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderText(string &TextToDisplay,DesignObjText *Text,TextStyle *pTextStyle, PlutoPoint point)
{

    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);

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
		pTextStyle,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment, &m_mapTextStyle);

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{
	//clipping
	if(X + Width >= m_iImageWidth)
		Width = m_iImageWidth - X - 1;

	if(Y + Height >= m_iImageHeight)
		Height = m_iImageHeight - Y - 1;

	sge_Rect(m_pScreenImage,X,Y,Width + X,Height + Y,color.m_Value);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity /*= 100*/)
{
	//clipping
	if(x + width >= m_iImageWidth)
		width = m_iImageWidth - x - 1;

	if(y + height >= m_iImageHeight)
		height = m_iImageHeight - y - 1;

	SDL_Rect Rectangle;
	Rectangle.x = x; Rectangle.y = y; Rectangle.w = width; Rectangle.h = height;

    X_LockDisplay();
	SDL_FillRect(m_pScreenImage, &Rectangle, SDL_MapRGBA(m_pScreenImage->format, color.R(), color.G(), color.B(), color.A()));
    X_UnlockDisplay();
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::RenderGraphic(PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal,
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

            X_LockDisplay();
			SDL_BlitSurface(rotozoom_picture, NULL, m_pScreenImage, &Destination);
			SDL_FreeSurface(rotozoom_picture);
            X_UnlockDisplay();
		}
		else //same size ... just blit the surface
        {
            X_LockDisplay();
            SDL_BlitSurface(pSDL_Surface, NULL, m_pScreenImage, &Destination);
            X_UnlockDisplay();
        }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	if (!EnableOpenGL)
	{
		SDL_Rect SourceRect;
		SourceRect.x = point.X + pObj->m_rPosition.Left(); SourceRect.y = point.Y + pObj->m_rPosition.Top();
		SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;
		
        SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
             pObj->m_rPosition.Width, pObj->m_rPosition.Height, 32, rmask, gmask, bmask, amask);
		
        X_LockDisplay();
		SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
		SDL_SetAlpha(pSDL_Surface, SDL_RLEACCEL , SDL_ALPHA_OPAQUE);
        X_UnlockDisplay();
        
		pObj->m_pGraphicToUndoSelect = new SDLGraphic(pSDL_Surface);
	}
}

PlutoGraphic *OrbiterSDL::GetBackground( PlutoRectangle &rect )
{
	if (!EnableOpenGL)
	{
		//clipping
		if(rect.X + rect.Width >= m_iImageWidth)
			rect.Width = m_iImageWidth - rect.X - 1;

		if(rect.Y + rect.Height >= m_iImageHeight)
			rect.Height = m_iImageHeight - rect.Y - 1;


        SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			rect.Width, rect.Height, 32, rmask, gmask, bmask, amask);

		SDL_Rect SourceRect;
		SourceRect.x = rect.Left(); SourceRect.y = rect.Top();
		SourceRect.w = rect.Width; SourceRect.h = rect.Height;

        X_LockDisplay();
		SDL_SetAlpha(m_pScreenImage, 0, 0);
		SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
        X_UnlockDisplay();
        
		return new SDLGraphic(pSDL_Surface);
	}
	else
		return new SDLGraphic(this);
}

void OrbiterSDL::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}

void OrbiterSDL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
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

/*virtual*/ void OrbiterSDL::BeginPaint()
{
}

/*virtual*/ void OrbiterSDL::EndPaint()
{
    //if we are using a buffer surface to blit images and text (Bluetooth_Dongle uses this)
    //will have to update the hole screen
    //if not, the user will call UpdateRect function for each rectangle he must invalidate
#ifndef USE_ONLY_SCREEN_SURFACE
    DisplayImageOnScreen(m_pScreenImage);
#endif

	if(EnableOpenGL)
	{
#ifndef DISABLE_OPENGL
		CallMaintenanceInMiliseconds(0, (DCE::OrbiterCallBack)&OrbiterSDL::OpenGLUpdateScreen, NULL, pe_NO);
#endif
	}
}

/*virtual*/ void OrbiterSDL::OnQuit()
{
    g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
    m_bQuit = true;
    pthread_cond_broadcast( &m_listMessageQueueCond );
    SDL_Event *pEvent = new SDL_Event;
    pEvent->type = SDL_QUIT;
    SDL_PushEvent(pEvent);
    delete pEvent;
}

/*virtual*/ PlutoGraphic *OrbiterSDL::CreateGraphic()
{
    return new SDLGraphic(this);
}

/*virtual*/ void OrbiterSDL::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
    //clipping the rectangle
    PlutoRectangle localrect = rect;
    localrect.X += point.X;
    localrect.Y += point.Y;

    if(localrect.X < 0)
        localrect.X = 0;

    if(localrect.Y < 0)
        localrect.Y = 0;

    if(localrect.Right() >= m_Width)
        localrect.Width = m_Width - rect.X - 1;

    if(localrect.Bottom() >= m_Height)
        localrect.Height = m_Height - localrect.Y - 1;

#ifdef USE_ONLY_SCREEN_SURFACE
	if(!EnableOpenGL)
	{
	    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);

        X_LockDisplay();
        SDL_UpdateRect(Screen, localrect.Left(), localrect.Top(), localrect.Width, localrect.Height);
        X_UnlockDisplay();
    }
#endif
}

void OrbiterSDL::OnIdle()
{
#ifndef DISABLE_OPENGL
	if(EnableOpenGL)
	{
        WakeupFromCondWait();
	}
#endif
    Sleep(5);
}

void OrbiterSDL::WakeupFromCondWait()
{
#ifndef DISABLE_OPENGL
	pthread_cond_broadcast(&m_GLThreadCond);
#endif
}

void OrbiterSDL::DoHighlightObject()
{
	if(!EnableOpenGL)
	{
		Orbiter::DoHighlightObject();
	}
	else
	{
#ifndef DISABLE_OPENGL
		DoHighlightObjectOpenGL();
#endif
	}
}

void OrbiterSDL::DoHighlightObjectOpenGL()
{
#ifndef DISABLE_OPENGL

	if(sbNoSelection == m_nSelectionBehaviour)
		return;

	PLUTO_SAFETY_LOCK( cm, m_ScreenMutex );  // Protect the highlighed object
	if( m_pGraphicBeforeHighlight )
		UnHighlightObject();

	if( !m_pObj_Highlighted )
		return;

	if( m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) m_pObj_Highlighted;
		PLUTO_SAFETY_LOCK( dg, m_DatagridMutex );

		int nHColumn = pGrid->m_iHighlightedColumn!=-1 ? pGrid->m_iHighlightedColumn + pGrid->m_GridCurCol : pGrid->m_GridCurCol;
		int nHRow = pGrid->m_iHighlightedRow!=-1 ? pGrid->m_iHighlightedRow + pGrid->m_GridCurRow - (pGrid->m_iUpRow >= 0 ? 1 : 0) : 0;

		if( nHColumn==-1 && nHRow==-1 )
			return;

		if(!pGrid->m_pDataGridTable)
			return;

		if(nHRow < pGrid->m_pDataGridTable->m_StartingRow)
		{
			pGrid->m_iHighlightedRow = 1;
			nHRow = pGrid->m_pDataGridTable->m_StartingRow; //set the highlighted row
		}

		DataGridCell *pCell = pGrid->m_pDataGridTable->GetData(nHColumn, nHRow); 
		if( !pCell )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter::DoHighlightObject cell is null.  obj %s col %d row %d",
				m_pObj_Highlighted->m_ObjectID.c_str(), nHColumn, nHRow);
			return;

		}

		//the datagrid is highlighted, but no row is highlighted; we don't want to select the whole datagrid
		if(pGrid->m_iHighlightedRow == -1)
			pGrid->m_iHighlightedRow = 0;

		PlutoRectangle r;
		GetGridCellDimensions( pGrid,  
			pGrid->m_iHighlightedColumn==-1 ? pGrid->m_MaxCol : pCell->m_Colspan, 
			pGrid->m_iHighlightedRow==-1 ? pGrid->m_MaxRow : pCell->m_Rowspan,
			pGrid->m_iHighlightedColumn==-1 ? 0 : pGrid->m_iHighlightedColumn, 
			pGrid->m_iHighlightedRow==-1 ? 0 : pGrid->m_iHighlightedRow, 
			r.X,  r.Y,  r.Width,  r.Height );

		m_rectLastHighlight.X = max(0,r.X);
		m_rectLastHighlight.Y = max(0,r.Y);
		m_rectLastHighlight.Right( min(r.Right(),m_Width-1) );
		m_rectLastHighlight.Bottom( min(r.Bottom(),m_Height-1) );
	}
	else
		m_rectLastHighlight = m_pObj_Highlighted->GetHighlightRegion();

	m_rectLastHighlight.X += m_pObj_Highlighted->m_pPopupPoint.X;
	m_rectLastHighlight.Y += m_pObj_Highlighted->m_pPopupPoint.Y;

	m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	m_rectLastHighlight.Height++;

	FloatRect HighLightArea;
	HighLightArea.Left = (float)m_rectLastHighlight.Left();
	HighLightArea.Top = (float)m_rectLastHighlight.Top();
	HighLightArea.Width =  (float)m_rectLastHighlight.Width;
	HighLightArea.Height = (float)m_rectLastHighlight.Height;

	Orbiter3DCommons::GetInstance()->SetHighLightArea(&HighLightArea);

	GL2DEffect * Effect = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_HIGHLIGHT_AREA, 
		Simulator::GetInstance()->m_iMilisecondsHighLight);
	if(Effect)
		Effect->Configure(&m_rectLastHighlight);

#endif //opengl stuff
}

void OrbiterSDL::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if (!EnableOpenGL) 
	{
		Orbiter::SelectObject(pObj, point);
		return;
	}

#ifndef DISABLE_OPENGL

	FloatRect SelectedArea;
	SelectedArea.Left   = (float)point.X + pObj->m_rBackgroundPosition.X;
	SelectedArea.Top    = (float)(point.Y + pObj->m_rBackgroundPosition.Y);
	SelectedArea.Width  = (float)pObj->m_rBackgroundPosition.Width;
	SelectedArea.Height = (float)pObj->m_rBackgroundPosition.Height;

	Orbiter3DCommons::GetInstance()->SetSelectedArea(&SelectedArea);
	GL2DEffect * Effect = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_SELECT_AREA, 
		Simulator::GetInstance()->m_iMilisecondsHighLight);

	if(!Effect)
		return;
	PlutoRectangle SeclectedAreaEffectSize;
	SeclectedAreaEffectSize.X = point.X + pObj->m_rBackgroundPosition.X;
	SeclectedAreaEffectSize.Y = point.Y + pObj->m_rBackgroundPosition.Y;
	SeclectedAreaEffectSize.Width = pObj->m_rBackgroundPosition.Width;
	SeclectedAreaEffectSize.Height = pObj->m_rBackgroundPosition.Height;
		
	Effect->Configure(&SeclectedAreaEffectSize);

#endif //opengl stuff
}  

/*virtual*/ void OrbiterSDL::OpenGLUpdateScreen(void* Data)
{
#ifndef DISABLE_OPENGL
	//the surface after the screen was rendered
	if(m_spAfterGraphic.get())
		m_spAfterGraphic->Initialize();

	m_spAfterGraphic.reset(new SDLGraphic(m_pScreenImage));

	m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
	GL2DEffect * Effect = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_NOEFFECT, 
		1);
#endif //opengl stuff	
}
