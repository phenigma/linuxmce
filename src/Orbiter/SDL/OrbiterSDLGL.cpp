/*
 OrbiterSDLGL

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
#include "OrbiterSDLGL.h"
#include "SerializeClass/ShapesColors.h"

#include "PlutoSDLDefs.h"
#include "../RendererOCG.h"
#include "SDLRendererOCGHelper.h"

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
#include <memory>

#if !defined(BLUETOOTH_DONGLE) && !defined(PROXY_ORBITER)
#define USE_ONLY_SCREEN_SURFACE
#endif

#ifdef WINCE
    #include "wince.h"
#endif

#include "../OpenGL/math3dutils.h"
#include "../OpenGL/GL2DWidgets/basicwindow.h"
#include "../OpenGL/GL2DWidgets/DrawingWidgetsEngine.h"

#include "../OpenGL/orbitergl3dengine.h"

#include "../OpenGL/GL2DEffects/gl2deffecttransit.h"
#include "../OpenGL/GL2DEffects/gl2deffectbeziertranzit.h"
#include "../OpenGL/GL2DEffects/gl2deffectslidefromleft.h"
#include "../OpenGL/GL2DEffects/gl2deffectfadesfromtop.h"
#include "../OpenGL/GL2DEffects/gl2deffectfadesfromunderneath.h"

namespace DCE
{

bool PaintDesktop;

void *Orbiter_OpenGLThread(void *p)
{
	OrbiterSDLGL * pOrbiterSDLGL = (OrbiterSDLGL *) p;
	pOrbiterSDLGL->m_Desktop = new OrbiterGL3D(pOrbiterSDLGL);
	
	PaintDesktop = false;

	PLUTO_SAFETY_LOCK(cm, pOrbiterSDLGL->m_GLThreadMutex);
	while(!pOrbiterSDLGL->m_bQuit)
	{
		//nothing to process. let's sleep...
		cm.CondWait(); // This will unlock the mutex and lock it on awakening

		if (!pOrbiterSDLGL->m_Desktop->EffectBuilder->HasEffects()) {
			if (!PaintDesktop) {
				PaintDesktop = true;
				pOrbiterSDLGL->m_Desktop->Paint();
			}
			cm.Release();
			Sleep(10);
			cm.Relock();
		}
		else {
			PaintDesktop = false;
			
			pOrbiterSDLGL->m_Desktop->Paint();
			cout<<"a frame"<<endl;
		}
	}
	
	delete pOrbiterSDLGL->m_Desktop;
	cout<<"Quits from OpenGL"<<endl;
	
	return NULL;
}	



OrbiterSDLGL::OrbiterSDLGL(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory,
bool bLocalMode, int nImageWidth, int nImageHeight, bool bFullScreen/*=false*/,
pluto_pthread_mutex_t *pExternalScreenMutex/*=NULL*/)
: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth,
nImageHeight, pExternalScreenMutex) ,  m_GLThreadMutex("open gl worker thread")
{
	m_pScreenImage = NULL;
	m_bFullScreen=bFullScreen;

	m_Desktop = NULL;

	pthread_cond_init(&m_GLThreadCond, NULL);
	m_GLThreadMutex.Init(NULL, &m_GLThreadCond);

	g_pPlutoLogger->Write(LV_CRITICAL, "Condition for our thread? %p", m_GLThreadMutex.m_pthread_cond_t);
}

//----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterSDLGL::GetConfig()
{
    if(!Orbiter::GetConfig())
        return false;

	pthread_t SDLGLthread;
	pthread_create(&SDLGLthread, NULL, Orbiter_OpenGLThread, (void*)this);

    m_pScreenImage = SDL_CreateRGBSurface(SDL_SWSURFACE, m_iImageWidth, m_iImageHeight, 32, rmask, gmask, bmask, amask);
    if (m_pScreenImage == NULL) {
        g_pPlutoLogger->Write(LV_WARNING, "SDL_CreateRGBSurface failed! %s",SDL_GetError());
    }

    return true;
}
//----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterSDLGL::~OrbiterSDLGL()
{
    KillMaintThread(); // We need to do this before freeing the surface.  It's a repeat of what's in Orbiter's destructor
g_pPlutoLogger->Write(LV_STATUS, "about to free surface");

#ifndef USE_ONLY_SCREEN_SURFACE
    SDL_FreeSurface(m_pScreenImage);
#endif

    m_pScreenImage = NULL;

    pthread_mutex_destroy(&m_GLThreadMutex.mutex);
    
g_pPlutoLogger->Write(LV_STATUS, "~OrbiterSDLGL finished");
}
//-----------------------------------------------------------------------------------------------------

/**
 * FirstEffects
 */
/*virtual*/ void OrbiterSDLGL::RenderScreen()
{
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
#endif

	PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
	PLUTO_SAFETY_LOCK(glm, m_GLThreadMutex);
	PlutoRectangle rectLastSelected(0, 0, 0, 0);

	//saving the surface before rendering the screen
	SDL_Surface *pBeforeRender = SDL_CreateRGBSurface(SDL_SWSURFACE, m_iImageWidth, m_iImageHeight, 32, 
		rmask, gmask, bmask, amask);
	m_spBeforeGraphic.reset(new SDLGraphic(pBeforeRender));
	SDL_BlitSurface(m_pScreenImage, NULL, pBeforeRender, NULL);

	if (m_pScreenHistory_Current)
		SDL_FillRect(m_pScreenImage, NULL, SDL_MapRGBA(m_pScreenImage->format, 0, 0, 0, 255));

	//screen rendering
	Orbiter::RenderScreen();

	//the surface after the screen was rendered
	if(m_spAfterGraphic.get())
		m_spAfterGraphic->Initialize();

	m_spAfterGraphic.reset(new SDLGraphic(m_pScreenImage));
	
    if(m_pObj_SelectedLastScreen)
	{
    	rectLastSelected = PlutoRectangle(m_pObj_SelectedLastScreen->m_rPosition);
		rectLastSelected.Y = m_iImageHeight - m_pObj_SelectedLastScreen->m_rPosition.Y;
	}
    else
	{
    	g_pPlutoLogger->Write(LV_CRITICAL, "No object selected? :o");
		rectLastSelected.X = 0;
		rectLastSelected.Y = m_iImageHeight;
		rectLastSelected.Width = 80;
		rectLastSelected.Height = 80;
	}

	//TODO: fix me
	m_pObj_SelectedLastScreen = NULL;
    
    g_pPlutoLogger->Write(LV_CRITICAL, "Last selected object rectangle : x %d, y %d, w %d, h %d",
    	rectLastSelected.X, rectLastSelected.Y, rectLastSelected.Width, rectLastSelected.Height);

    if(m_Desktop && !m_Desktop->EffectBuilder->HasEffects())
    {
		//cout<<"One effect was created!";
		
 		//GL2DBezierEffectTransit* Transit = (GL2DBezierEffectTransit*) m_Desktop->EffectBuilder->
		//	CreateEffect(GL2D_EFFECT_BEZIER_TRANSIT, 900);

		GL2DEffectFadesFromUnderneath* Transit = (GL2DEffectFadesFromUnderneath*) m_Desktop->EffectBuilder->
			CreateEffect(GL2D_EFFECT_FADES_FROM_UNDERNEATH, 900);

		Transit->Configure(m_spBeforeGraphic.get(), m_spAfterGraphic.get()/*, rectLastSelected*/);
    } 

    glm.Release();
    DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::DisplayImageOnScreen(SDL_Surface *m_pScreenImage)
{
    //g_pPlutoLogger->Write(LV_CRITICAL, "GL Display Image");

    
	WakeupFromCondWait();
	
   //SDL_UpdateRect(Screen, 0, 0, 0, 0);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::RedrawObjects()
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
    Orbiter::RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::RenderText(string &TextToDisplay,DesignObjText *Text,TextStyle *pTextStyle, PlutoPoint point)
{
    SDL_Rect TextLocation;
    TextLocation.x = point.X + Text->m_rPosition.X;
    TextLocation.y = point.Y + Text->m_rPosition.Y;
    TextLocation.w = Text->m_rPosition.Width;
    TextLocation.h = Text->m_rPosition.Height;

#ifdef WIN32
    string BasePath="C:\\Windows\\Fonts\\";
#else
    string BasePath="/usr/pluto/fonts/";
#endif //win32

    try
    {
        WrapAndRenderText(m_pScreenImage, TextToDisplay, TextLocation.x, TextLocation.y, TextLocation.w, TextLocation.h, BasePath,
            pTextStyle,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment, &m_mapTextStyle);
    }
    catch(...)
    {
    }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{
	//clipping
	if(X + Width >= m_iImageWidth)
		Width = m_iImageWidth - X - 1;

	if(Y + Height >= m_iImageHeight)
		Height = m_iImageHeight - Y - 1;

    sge_Rect(m_pScreenImage,X,Y,Width + X,Height + Y,color.m_Value);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity /*= 100*/)
{
	//clipping
	if(x + width >= m_iImageWidth)
		width = m_iImageWidth - x - 1;

	if(y + height >= m_iImageHeight)
		height = m_iImageHeight - y - 1;

    SDL_Rect Rectangle;
    Rectangle.x = x; Rectangle.y = y; Rectangle.w = width; Rectangle.h = height;

    SDL_FillRect(m_pScreenImage, &Rectangle, SDL_MapRGBA(m_pScreenImage->format, color.R(), color.G(), color.B(), color.A()));
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::RenderGraphic(PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal,
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

            SDL_BlitSurface(rotozoom_picture, NULL, m_pScreenImage, &Destination);
            SDL_FreeSurface(rotozoom_picture);
        }
        else //same size ... just blit the surface
            SDL_BlitSurface(pSDL_Surface, NULL, m_pScreenImage, &Destination);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
    SDL_Rect SourceRect;
    SourceRect.x = point.X + pObj->m_rPosition.Left(); SourceRect.y = point.Y + pObj->m_rPosition.Top();
    SourceRect.w = pObj->m_rPosition.Width; SourceRect.h = pObj->m_rPosition.Height;

    SDL_Surface *pSDL_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
        pObj->m_rPosition.Width, pObj->m_rPosition.Height, 32, rmask, gmask, bmask, amask);

    SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);
    SDL_SetAlpha(pSDL_Surface, SDL_RLEACCEL , SDL_ALPHA_OPAQUE);

    pObj->m_pGraphicToUndoSelect = new SDLGraphic(pSDL_Surface);
}

PlutoGraphic *OrbiterSDLGL::GetBackground( PlutoRectangle &rect )
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

    SDL_SetAlpha(m_pScreenImage, 0, 0);
    SDL_BlitSurface(m_pScreenImage, &SourceRect, pSDL_Surface, NULL);

    return new SDLGraphic(pSDL_Surface);
}

//-----------------------------------------------------------------------------------------------------
void OrbiterSDLGL::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
    Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}

//-----------------------------------------------------------------------------------------------------
void OrbiterSDLGL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
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
/*virtual*/ void OrbiterSDLGL::BeginPaint()
{
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::EndPaint()
{
    //if we are using a buffer surface to blit images and text (Bluetooth_Dongle uses this)
    //will have to update the hole screen
    //if not, the user will call UpdateRect function for each rectangle he must invalidate
#ifndef USE_ONLY_SCREEN_SURFACE
    DisplayImageOnScreen(m_pScreenImage);
#endif
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::OnQuit()
{
    g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
    m_bQuit = true;
    pthread_cond_broadcast( &m_listMessageQueueCond );
    SDL_Event *pEvent = new SDL_Event;
    pEvent->type = SDL_QUIT;
    SDL_PushEvent(pEvent);
    delete pEvent;
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDLGL::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{

}

void OrbiterSDLGL::OnIdle()
{
	WakeupFromCondWait();
}

void OrbiterSDLGL::WakeupFromCondWait()
{
	g_pPlutoLogger->Write(LV_STATUS, "WakeupFromCondWait");
	Sleep(10);
	pthread_cond_broadcast(&m_GLThreadCond);
}

/*virtual*/ PlutoGraphic *OrbiterSDLGL::CreateGraphic()
{
	return new SDLGraphic(this);
}

}
//-----------------------------------------------------------------------------------------------------

