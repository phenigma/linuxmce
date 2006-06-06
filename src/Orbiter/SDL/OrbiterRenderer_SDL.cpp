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

using namespace DCE;

bool g_bResettingVideoMode;
void *HackThread2(void *p)
{
    OrbiterRenderer_SDL *pOrbiterRenderer_SDL = (OrbiterRenderer_SDL *) p;
    g_pPlutoLogger->Write(LV_STATUS,"Inside Hacktrhead #2");
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

#ifndef DISABLE_OPENGL

/**
 * OpenGL drawing thread. It is in three steps: 
 * - Init OpenGL with SDL
 * - looping to drawing frames
 * - end the OpenGL
 */


void *Orbiter_OpenGLThread(void *p)
{
	OrbiterRenderer_SDL * pOrbiterRenderer = (OrbiterRenderer_SDL*) p;
	// Create the OpenGL window and classes asociated with it
	// OpenGL drawing operations, locked in the mutex
	PLUTO_SAFETY_LOCK(cm, *(pOrbiterRenderer->m_GLThreadMutex));

	pOrbiterRenderer->m_Desktop = OrbiterGL3D::GetInstance();
	pOrbiterRenderer->m_Desktop->BuildOrbiterGL(pOrbiterRenderer);
	
	/*
	size_t iSize;
	SDLGraphic *pCursorGraphic = NULL;
	char * pData = FileUtils::ReadFileIntoBuffer("/home/pluto_cursor.png", iSize);
	if (pData)
	{
		pCursorGraphic = (SDLGraphic*)pOrbiterRenderer->CreateGraphic();
		pCursorGraphic->LoadGraphic(pData, iSize);
		delete [] pData;
	}

	Commons3D::Instance().SetMouseCursor(pCursorGraphic);
	*/

	cm.CondWait(); // This will unlock the mutex and lock it on awakening
	
	pOrbiterRenderer->PaintDesktopGL = false;

	while(!pOrbiterRenderer->OrbiterLogic()->m_bQuit)
	{
		//nothing to process. let's sleep...
		cm.CondWait(); // This will unlock the mutex and lock it on awakening

		/// Thread is waken up, for more reasons as Quit event or there is one effect pending or 
		/// is idle event
		// Orbiter is in Quit state, end the thread
		if(pOrbiterRenderer->OrbiterLogic())
			return NULL;

		if (!pOrbiterRenderer->m_Desktop->EffectBuilder->HasEffects()) 
		{
			if (!pOrbiterRenderer->PaintDesktopGL) 
			{
				pOrbiterRenderer->PaintDesktopGL = true;
				pOrbiterRenderer->m_Desktop->Paint();
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
			pOrbiterRenderer->PaintDesktopGL = false;
			pOrbiterRenderer->m_Desktop->Paint();
		}
	}

	delete pOrbiterRenderer->m_Desktop;

	return NULL;
}	
#endif

//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_SDL::OrbiterRenderer_SDL(Orbiter *pOrbiter) : OrbiterRenderer(pOrbiter)
{
	// Use OpenGL if is requested
	EnableOpenGL = false;

#ifdef DISABLE_OPENGL
	EnableOpenGL = false;
#endif

	m_spPendingGLEffects.reset(new PendingGLEffects());

	m_pScreenImage = NULL;
	//m_bFullScreen=bFullScreen;
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

OrbiterRenderer_SDL::~OrbiterRenderer_SDL()
{
	// We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
	g_pPlutoLogger->Write(LV_STATUS, "about to free surface");

	#ifndef USE_ONLY_SCREEN_SURFACE
		SDL_FreeSurface(m_pScreenImage);
	#endif

	m_pScreenImage = NULL;

	g_pPlutoLogger->Write(LV_STATUS, "~OrbiterRenderer_SDL finished");

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

void OrbiterRenderer_SDL::Configure()
{
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
	
        // TODO: move in StartOrbiterRenderer_SDL.cpp : init_system()
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
		pthread_create(&hackthread, NULL, HackThread2, (void*)this);

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
	} //if (!EnableOpenGL)
	else
	{
#ifndef DISABLE_OPENGL
		/// if OpenGL is used there is created one OpenGL drawing thread 
		pthread_create(&SDLGLthread, NULL, Orbiter_OpenGLThread, (void*)this);
		/// creates 2D surface which is drawed with 2D engine the Orbiter
		/// which lately will be used as a texture
        m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 32,
            rmask, gmask, bmask, amask);
		if (m_pScreenImage == NULL) {
			g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
		}
#endif
	}
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ObjectOnScreen(VectDesignObj_Orbiter *pVectDesignObj_Orbiter, 
	DesignObj_Orbiter *pObj, PlutoPoint *ptPopup/* = NULL*/)
{
	if(pObj->m_PK_Effect_On_Screen > 0)
		m_spPendingGLEffects->m_nOnScreenTransitionEffectID = pObj->m_PK_Effect_On_Screen;

	OrbiterRenderer::ObjectOnScreen(pVectDesignObj_Orbiter, pObj, ptPopup);
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::ObjectOffScreen(DesignObj_Orbiter *pObj)
{
	if(pObj->m_PK_Effect_Off_Screen > 0)
        m_spPendingGLEffects->m_nOffScreenTransitionEffectID = pObj->m_PK_Effect_Off_Screen;

	OrbiterRenderer::ObjectOffScreen(pObj);
}
//----------------------------------------------------------------------------------------------------
void OrbiterRenderer_SDL::RenderScreen( bool bRenderGraphicsOnly )
{
	#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",
			(OrbiterLogic()->m_pScreenHistory_Current ? OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
	#endif
	
	if (!EnableOpenGL)
	{
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
	else //EnableOpenGL
	{
#ifndef DISABLE_OPENGL
		PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);
		PLUTO_SAFETY_LOCK(glm, *m_GLThreadMutex);
		PlutoRectangle rectLastSelected(0, 0, 0, 0);
	
		//saving the surface before rendering the screen
        LockDisplay();
        SDL_Surface *pBeforeRender = SDL_CreateRGBSurface(SDL_HWSURFACE, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 32,
             rmask, gmask, bmask, amask);
        UnlockDisplay();
			
		SDL_Rect ImageSize;
		ImageSize.x = 0;
		ImageSize.y = 0;
		ImageSize.w = OrbiterLogic()->m_iImageWidth;
		ImageSize.h = OrbiterLogic()->m_iImageHeight;
        LockDisplay();
        SDL_BlitSurface(m_pScreenImage, NULL, pBeforeRender, NULL);
        UnlockDisplay();
	
		m_spBeforeGraphic.reset(new SDLGraphic(pBeforeRender));
		if (OrbiterLogic()->m_pScreenHistory_Current)
        {
            LockDisplay();
            SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));
            UnlockDisplay();
        }
	
		//screen rendering
		OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);
	
		//the surface after the screen was rendered
		if(m_spAfterGraphic.get())
			m_spAfterGraphic->Initialize();
	
		m_spAfterGraphic.reset(new SDLGraphic(m_pScreenImage));
		
		if(OrbiterLogic()->m_pObj_SelectedLastScreen)
		{
			rectLastSelected = PlutoRectangle(OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition);
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

		if(OrbiterLogic()->m_pObj_SelectedLastScreen)
			m_spPendingGLEffects->m_nOnSelectWithChangeEffectID = OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange;


		//TODO: Ciprian
		//You have here:
		int a = m_spPendingGLEffects->m_nOffScreenTransitionEffectID;
		int b = m_spPendingGLEffects->m_nOnScreenTransitionEffectID;
		int c = m_spPendingGLEffects->m_nOnSelectWithChangeEffectID;
		


		//////////////////////////////////////////////////////////////////////////
			//TODO: this is temporary
		if(OrbiterLogic()->UsesUIVersion2() && NULL != m_Desktop && NULL != m_Desktop->EffectBuilder)
		{
			g_pPlutoLogger->Write(LV_WARNING, "ConfigureNextScreen");
			m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());

			if(bRenderGraphicsOnly)
			{
				/*
				g_pPlutoLogger->Write(LV_WARNING, "bRenderGraphicsOnly is true; switching to no effect...");

				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
					GL2D_EFFECT_TRANSIT_NO_EFFECT,
					Simulator::GetInstance()->m_iMilisecondsTransition
					);
				*/

				g_pPlutoLogger->Write(LV_WARNING, "bRenderGraphicsOnly is true; no open gl effect will be rendered...");

			}
			else
			{
				int nCurrentDesignObjID = OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
				PlutoRectangle rectStartEffect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);
				int nPK_Effect = 0;
				int nTransitionTimeIsMs = Simulator::GetInstance()->m_iMilisecondsTransition; 

				static int nOldCurrentDesignObjID = 0;
				switch(nCurrentDesignObjID) 
				{
				case DESIGNOBJ_mnuMenu2_CONST:
					nPK_Effect = EFFECT_Basic_transit_effect_CONST;
					break;

				case DESIGNOBJ_mnuAmbiance_CONST:
					rectStartEffect.Location(PlutoPoint(0, OrbiterLogic()->m_iImageHeight - 40));
					rectStartEffect.Size(PlutoSize(100, 40));
					nPK_Effect = EFFECT_Slide_from_left_CONST;
					break;

				case DESIGNOBJ_mnuSpeedControl_CONST:
					rectStartEffect.Location(PlutoPoint(0, OrbiterLogic()->m_iImageHeight - 40));
					rectStartEffect.Size(PlutoSize(100, 40));
					nPK_Effect = EFFECT_Slide_from_top_CONST;
					break;

				case DESIGNOBJ_popFileList_CONST:
					nPK_Effect = EFFECT_Bezier_transit_prism_CONST;
					break;

				default:
					break;
				}

				if(nOldCurrentDesignObjID == nCurrentDesignObjID && nCurrentDesignObjID != DESIGNOBJ_mnuMenu2_CONST)
				{
					g_pPlutoLogger->Write(LV_WARNING, "Same screen; switching to no effect...");
					nPK_Effect = EFFECT_No_transit_effect_CONST;
				}
				else
					g_pPlutoLogger->Write(LV_WARNING, "We'll render a nice effect %d", nPK_Effect);

				nOldCurrentDesignObjID = nCurrentDesignObjID;

				//if(nPK_Effect != 0)
				{
					GL2DEffect* Transit = m_Desktop->EffectBuilder->
						CreateEffect(
						m_Desktop->EffectBuilder->GetEffectCode(nPK_Effect),
						nTransitionTimeIsMs
						);

					if(Transit)
						Transit->Configure(&rectStartEffect);
					else
						Transit = m_Desktop->EffectBuilder->
						CreateEffect(
						GL2D_EFFECT_TRANSIT_NO_EFFECT,
						Simulator::GetInstance()->m_iMilisecondsTransition
						);
				}
				//else
				//	g_pPlutoLogger->Write(LV_WARNING, "No opengl effect will be rendered!");
			}

			glm.Release();
			m_spPendingGLEffects->Reset();

			g_pPlutoLogger->Write(LV_WARNING, "Render screen returning...");
			return;
		}
		//////////////////////////////////////////////////////////////////////////


		if(OrbiterLogic()->m_pObj_SelectedLastScreen)
		{
			//m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange = rand() % 9 + 1;

			//Create transtition effect if there is no effect already pending
			if(m_Desktop && m_Desktop->EffectBuilder)
			{
				m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
						m_Desktop->EffectBuilder->GetEffectCode(OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange),
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
			m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
			m_Desktop->EffectBuilder->
				CreateEffect(
				GL2D_EFFECT_TRANSIT_NO_EFFECT,
				0
			);
		}
	
		glm.Release();
#endif
    }

	m_spPendingGLEffects->Reset();
}

void OrbiterRenderer_SDL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
	if(OrbiterLogic()->m_bQuit)
		return;

	if(!EnableOpenGL)
	{
		PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);
        LockDisplay();
	
	#ifndef USE_ONLY_SCREEN_SURFACE
        SDL_BlitSurface(m_pScreenImage, NULL, Screen, NULL);
	#endif
	
		SDL_UpdateRect(Screen, 0, 0, 0, 0);
        UnlockDisplay();
	}
	else
	{
#ifndef DISABLE_OPENGL
		WakeupFromCondWait();
#endif
	}

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
	if (!EnableOpenGL)
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
}

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

void OrbiterRenderer_SDL::BeginPaint()
{
}

void OrbiterRenderer_SDL::EndPaint()
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
		OrbiterLogic()->CallMaintenanceInMiliseconds(0, (OrbiterCallBack) &Orbiter::RenderFrame, NULL, pe_NO);
#endif
	}
}

void OrbiterRenderer_SDL::RenderFrame(void* Data)
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

void OrbiterRenderer_SDL::OnQuit()
{
    g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
    SDL_Event *pEvent = new SDL_Event;
    pEvent->type = SDL_QUIT;
    SDL_PushEvent(pEvent);
    delete pEvent;
}

PlutoGraphic *OrbiterRenderer_SDL::CreateGraphic()
{
    return new SDLGraphic(this);
}

void OrbiterRenderer_SDL::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
    //clipping the rectangle
    PlutoRectangle localrect = rect;
    localrect.X += point.X;
    localrect.Y += point.Y;

	ClipRectangle(localrect);

#ifdef USE_ONLY_SCREEN_SURFACE
	if(!EnableOpenGL)
	{
	    PLUTO_SAFETY_LOCK(cm,OrbiterLogic()->m_ScreenMutex);

        LockDisplay();
        SDL_UpdateRect(Screen, localrect.Left(), localrect.Top(), localrect.Width, localrect.Height);
        UnlockDisplay();
    }
#endif
}

void OrbiterRenderer_SDL::OnIdle()
{
#ifndef DISABLE_OPENGL
	if(EnableOpenGL)
	{
        WakeupFromCondWait();
	}
#endif
    Sleep(5);
}

void OrbiterRenderer_SDL::WakeupFromCondWait()
{
#ifndef DISABLE_OPENGL
	pthread_cond_broadcast(&m_GLThreadCond);
#endif
}

void OrbiterRenderer_SDL::DoHighlightObject()
{
	g_pPlutoLogger->Write(LV_WARNING, "OrbiterRenderer_SDL::DoHighlightObject");

	if(!EnableOpenGL)
	{
		OrbiterRenderer::DoHighlightObject();
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, ">> OrbiterRenderer_SDL::DoHighlightObjectOpenGL");

#ifndef DISABLE_OPENGL
		DoHighlightObjectOpenGL();
#endif
	}
}

void OrbiterRenderer_SDL::DoHighlightObjectOpenGL()
{
#ifndef DISABLE_OPENGL

	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour)
		return;

	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	if( OrbiterLogic()->m_pGraphicBeforeHighlight )
		UnHighlightObject();

	if( !OrbiterLogic()->m_pObj_Highlighted )
		return;

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *)OrbiterLogic()->m_pObj_Highlighted;
		PLUTO_SAFETY_LOCK( dg, OrbiterLogic()->m_DatagridMutex );

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
				OrbiterLogic()->m_pObj_Highlighted->m_ObjectID.c_str(), nHColumn, nHRow);
			return;

		}

		//the datagrid is highlighted, but no row is highlighted; we don't want to select the whole datagrid
		if(pGrid->m_iHighlightedRow == -1)
			pGrid->m_iHighlightedRow = 0;

		PlutoRectangle r;
		pGrid->GetGridCellDimensions(
			pGrid->m_iHighlightedColumn==-1 ? pGrid->m_MaxCol : pCell->m_Colspan, 
			pGrid->m_iHighlightedRow==-1 ? pGrid->m_MaxRow : pCell->m_Rowspan,
			pGrid->m_iHighlightedColumn==-1 ? 0 : pGrid->m_iHighlightedColumn, 
			pGrid->m_iHighlightedRow==-1 ? 0 : pGrid->m_iHighlightedRow, 
			r.X,  r.Y,  r.Width,  r.Height );

		OrbiterLogic()->m_rectLastHighlight.X = max(0,r.X);
		OrbiterLogic()->m_rectLastHighlight.Y = max(0,r.Y);
		OrbiterLogic()->m_rectLastHighlight.Right( min(r.Right(),OrbiterLogic()->m_Width-1) );
		OrbiterLogic()->m_rectLastHighlight.Bottom( min(r.Bottom(),OrbiterLogic()->m_Height-1) );
	}
	else
		OrbiterLogic()->m_rectLastHighlight = OrbiterLogic()->m_pObj_Highlighted->GetHighlightRegion();

	OrbiterLogic()->m_rectLastHighlight.X += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.X;
	OrbiterLogic()->m_rectLastHighlight.Y += OrbiterLogic()->m_pObj_Highlighted->m_pPopupPoint.Y;

	OrbiterLogic()->m_rectLastHighlight.Width++;  // GetBackground always seems to be 1 pixel to little
	OrbiterLogic()->m_rectLastHighlight.Height++;

	FloatRect HighLightArea;
	HighLightArea.Left = (float)OrbiterLogic()->m_rectLastHighlight.Left();
	HighLightArea.Top = (float)OrbiterLogic()->m_rectLastHighlight.Top();
	HighLightArea.Width =  (float)OrbiterLogic()->m_rectLastHighlight.Width;
	HighLightArea.Height = (float)OrbiterLogic()->m_rectLastHighlight.Height;

	Commons3D::Instance().SetHighLightArea(&HighLightArea);

	g_pPlutoLogger->Write(LV_WARNING, ">> OrbiterRenderer_SDL::DoHighlightObjectOpenGL : GL2D_EFFECT_HIGHLIGHT_AREA");

	GL2DEffect * Effect = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_HIGHLIGHT_AREA, 
		Simulator::GetInstance()->m_iMilisecondsHighLight);

	if(Effect)
		Effect->Configure(&OrbiterLogic()->m_rectLastHighlight);
	else
		g_pPlutoLogger->Write(LV_CRITICAL, ">> OrbiterRenderer_SDL::DoHighlightObjectOpenGL : no effect???!");

#endif //opengl stuff
}

void OrbiterRenderer_SDL::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if (!EnableOpenGL) 
	{
		//TODO: 
		//Orbiter::SelectObject(pObj, point);
		return;
	}

#ifndef DISABLE_OPENGL

	FloatRect SelectedArea;
	SelectedArea.Left   = (float)point.X + pObj->m_rBackgroundPosition.X;
	SelectedArea.Top    = (float)(point.Y + pObj->m_rBackgroundPosition.Y);
	SelectedArea.Width  = (float)pObj->m_rBackgroundPosition.Width;
	SelectedArea.Height = (float)pObj->m_rBackgroundPosition.Height;

	Commons3D::Instance().SetSelectedArea(&SelectedArea);
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
				orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
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
/*virtual*/ bool OrbiterRenderer_SDL::DisplayProgress(string sMessage, int nProgress)
{
	return OrbiterRenderer::DisplayProgress(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer_SDL::DisplayProgress(string sMessage, 
const map<string, bool> &mapChildDevices, int nProgress)
{
	return OrbiterRenderer::DisplayProgress(sMessage, mapChildDevices, nProgress);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterRenderer_SDL::PromptUser(string sPrompt, int iTimeoutSeconds, 
map<int,string> *p_mapPrompts)
{
	return OrbiterRenderer::PromptUser(sPrompt, iTimeoutSeconds);
}
//-----------------------------------------------------------------------------------------------------
