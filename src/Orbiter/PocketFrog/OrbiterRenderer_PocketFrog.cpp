#include "OrbiterRenderer_PocketFrog.h"

#include "Wingdi.h"
#include "MainDialog.h"
#include "Resource.h"
#include "VirtualKeysTranslator.h"
#include "PopupMessage.h"
#include "ProgressDialog.h"
#include "ScreenHistory.h"
#include "Orbiter.h"

#ifdef ENABLE_MOUSE_BEHAVIOR
#include "Win32/MouseBehavior_Win32.h"
#endif

#include "../pluto_main/Define_Effect.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 
#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 

#include "../../DCE/DataGrid.h"

#ifndef WINCE //no opengl support in windows ce
	#include <GL/gl.h>

	#include "OpenGLProxy.h"
	#include "OpenGLTextureConverter_PocketFrog.h"
	#include "OpenGL/orbitergl3dengine.h"
	#include "OpenGL/math3dutils.h"
	#include "OpenGL/GL2DWidgets/basicwindow.h"
	#include "OpenGL/GL2DWidgets/DrawingWidgetsEngine.h"
	#include "OpenGL/orbitergl3dengine.h"
	#include "OpenGL/GL2DEffects/gl2deffecttransit.h"
	#include "OpenGL/GL2DEffects/gl2deffectbeziertranzit.h" 
	#include "../OpenGL/Orbiter3DCommons.h" 
	#include "../Simulator.h" 
#endif

using namespace Frog;
using namespace Frog::Internal;
#include <src/internal/graphicbuffer.h>
#include <src/rasterizer.h>
#include <src/internal/raster2d.h>
//-----------------------------------------------------------------------------------------------------
const int ciCharWidth = 9;
const int ciCharHeight = 16;
const int ciSpaceHeight = 5;
//-----------------------------------------------------------------------------------------------------
#define MAX_STRING_LEN 1024
#define MATCH_SDL_FONT_HEIGHT 9 / 8
#define TITLEBAR_HEIGHT 18
#define BORDER_WIDTH 3
//-----------------------------------------------------------------------------------------------------
using namespace Frog;
CComModule _Module;
//-----------------------------------------------------------------------------------------------------
// GDI Escapes for ExtEscape()
#define QUERYESCSUPPORT    8
 

// The following are unique to CE
#define GETVFRAMEPHYSICAL   6144
#define GETVFRAMELEN    6145
#define DBGDRIVERSTAT    6146
#define SETPOWERMANAGEMENT   6147
#define GETPOWERMANAGEMENT   6148
 
#define REG_BACKLIGHT L"ControlPanel\\Backlight"
#define REG_VAL_BATT_TO L"BatteryTimeout"
#define REG_VAL_AC_TO L"ACTimeout"
unsigned int OldBattBL=0;
unsigned int OldACBL=0;
//------------------------------------------------------------------------------------------------------------
typedef enum _VIDEO_POWER_STATE {
    VideoPowerOn = 1,
    VideoPowerStandBy,
    VideoPowerSuspend,
    VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;
//-----------------------------------------------------------------------------------------------------
typedef struct _VIDEO_POWER_MANAGEMENT {
    ULONG Length;
    ULONG DPMSVersion;
    ULONG PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;
//-----------------------------------------------------------------------------------------------------
#ifdef WINCE
	#define DT_MODIFYSTRING 0
	#define DT_END_ELLIPSIS 0
#endif
//-----------------------------------------------------------------------------------------------------
#define CHECK_STATUS() { if(OrbiterLogic()->m_bQuit) return; }
//-----------------------------------------------------------------------------------------------------
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
//no open gl support under CE
#ifdef WINCE
#define DISABLE_OPENGL
#endif
//-----------------------------------------------------------------------------------------------------
#ifndef DISABLE_OPENGL

/**
* OpenGL drawing thread. It is in three steps: 
* - Init OpenGL with SDL
* - looping to drawing frames
* - end the OpenGL
*/
void *Orbiter_OpenGLThread(void *p)
{
	OrbiterRenderer_PocketFrog * pOrbiterRenderer = (OrbiterRenderer_PocketFrog *) p;
	// Create the OpenGL window and classes asociated with it
	pOrbiterRenderer->m_Desktop = OrbiterGL3D::GetInstance();

	Orbiter *pOrbiter = pOrbiterRenderer->OrbiterLogic();
	pOrbiterRenderer->m_Desktop->BuildOrbiterGL(pOrbiterRenderer);
	pOrbiterRenderer->PaintDesktopGL = false;

	// OpenGL drawing operations, locked in the mutex
	PLUTO_SAFETY_LOCK(cm, *(pOrbiterRenderer->m_GLThreadMutex));
	while(!pOrbiter->m_bQuit)
	{
		//nothing to process. let's sleep...
		cm.CondWait(); // This will unlock the mutex and lock it on awakening

		/// Thread is waken up, for more reasons as Quit event or there is one effect pending or 
		/// is idle event
		// Orbiter is in Quit state, end the thread
		if(pOrbiter->m_bQuit)
			return NULL;

		if (!pOrbiterRenderer->m_Desktop->EffectBuilder->HasEffects()) {
			if (!pOrbiterRenderer->PaintDesktopGL) {
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
OrbiterRenderer_PocketFrog::OrbiterRenderer_PocketFrog(Orbiter *pOrbiter) :	OrbiterRenderer(pOrbiter)
{
	m_Desktop = NULL;
    m_config.orientation      = ORIENTATION_WEST;
    m_config.splashScreenTime = 0;	
	m_bUpdating = false;
	
	//TODO: hardcoded for now
	EnableOpenGL = false;

#ifdef WINCE
	EnableOpenGL = false; //opengl not available for WINCE
#endif

	m_bPoolRendering = true;
}
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_PocketFrog::~OrbiterRenderer_PocketFrog()
{
#ifndef WINCE
	if (EnableOpenGL && m_spAfterGraphic.get())
		m_spAfterGraphic->Initialize();
#endif
}

//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_PocketFrog::GameInit()
{
    ::SetWindowText(m_hWnd, TEXT("Pluto Orbiter"));

#ifndef WINCE
    ::SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX);
#endif
    
    PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);

	Rect r;
	::GetClientRect( m_hWnd, &r );
	Surface* pLogoSurface = NULL;

#ifdef WINCE
    string sLogoPath = g_sBinaryPath + "logo.gif";
    sLogoPath = StringUtils::Replace(sLogoPath, "/", "\\");

    wchar_t wPath[4096];
    mbstowcs(wPath, sLogoPath.c_str(), 4096);	

	pLogoSurface = LoadImage( GetDisplay(), wPath);
#else
	pLogoSurface = LoadImage( GetDisplay(), TEXT("logo.gif")); 
#endif

	if(pLogoSurface) 
	{
        if(pLogoSurface->m_height == OrbiterLogic()->m_iImageHeight && pLogoSurface->m_width == OrbiterLogic()->m_iImageWidth)
		    GetDisplay()->Blit( 0, 0, pLogoSurface );
        else //zoom
        {
            Rect dest;	
            dest.Set(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);

            double ZoomX = OrbiterLogic()->m_iImageWidth / double(pLogoSurface->GetWidth());
            double ZoomY = OrbiterLogic()->m_iImageHeight / double(pLogoSurface->GetHeight());

            dest.right = dest.left + int(pLogoSurface->GetWidth() * ZoomX);
            dest.bottom = dest.top + int(pLogoSurface->GetHeight() * ZoomY);     
            
            if( dest.right-dest.left>0 && dest.bottom-dest.top>0 )  // PF crashes with 0 width/height
                GetDisplay()->BlitStretch(dest, pLogoSurface); 
        }
	}
	else
	{
		GetDisplay()->FillRect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 0x00);
	}

	if(pLogoSurface)
		delete pLogoSurface;

	if (!EnableOpenGL)
	{
        GetDisplay()->Update();
	}
	else
	{
#ifndef DISABLE_OPENGL
		/// if OpenGL is used there is created one OpenGL drawing mutex
		m_GLThreadMutex = new pluto_pthread_mutex_t("open gl worker thread");

		pthread_cond_init(&m_GLThreadCond, NULL);
		m_GLThreadMutex->Init(NULL, &m_GLThreadCond);
		m_Desktop = NULL;

		/// if OpenGL is used there is created one OpenGL drawing thread 
		pthread_create(&SDLGLthread, NULL, Orbiter_OpenGLThread, (void*)this);

		// FIXME Wait to create all OpenGL objects
		while((!m_Desktop) || (m_Desktop && !m_Desktop->EffectBuilder))
			Sleep(10);

		m_Desktop->EffectBuilder->
			CreateEffect(
			GL2D_EFFECT_NOEFFECT,
			0
			);
		StartAnimation();
#endif
	}

	OrbiterLogic()->Initialize(gtPocketFrogGraphic);

	if(OrbiterLogic()->m_bQuit || OrbiterLogic()->m_bReload)
    {
		//we were just loading and someone did a reload router.
		//we'll start again
		OnUnexpectedDisconnect();
		return false;
    }

	if (!OrbiterLogic()->m_bLocalMode)
		OrbiterLogic()->CreateChildren();

#ifndef WINCE
	if(OrbiterLogic()->m_bFullScreen)
	{
		//set window style to fullscreen
		ModifyStyle(WS_CAPTION, 0);

		HWND hWnd_TaskBar = ::FindWindow("Shell_TrayWnd", NULL);
		if(hWnd_TaskBar)
			::SetWindowPos(hWnd_TaskBar, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); 

		RECT rc;
		HWND hWndDesktop = ::GetDesktopWindow();
		::GetWindowRect(hWndDesktop, &rc);
		MoveWindow(0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);

		BringWindowToTop();
		SetForegroundWindow(m_hWnd);

		Rect r;
		::GetClientRect( m_hWnd, &r );
		g_pPlutoLogger->Write(LV_CRITICAL, "After fullscreen GetClientRect reports: %d, %d, %d, %d",
			r.left, r.top, r.right, r.bottom);
	}
#endif

	return true;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::ShowWindowCustom()
{
#ifdef WINCE
	ShowWindow(SW_SHOWNORMAL);
#else
	HWND hMainWindow = ::FindWindow(TEXT("ORBITER"), NULL);

	if(!::IsIconic(hMainWindow)) //if main window is minimized, create minimized
		ShowWindow(SW_SHOWNORMAL);
	else
		ShowWindow(SW_MINIMIZE);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::GameEnd()
{
	
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::GameLoop()
{
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::StylusDown( Point p, MouseButton aMouseButton )
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, ">>> >>> OrbiterRenderer_PocketFrog::StylusDown(%d, %d, button: %d)",
		p.x, p.y, aMouseButton);
#endif

	OrbiterLogic()->RegionDown(p.x, p.y);
	RecordMouseAction(p.x, p.y);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::StylusUp( Point p, MouseButton aMouseButton )
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, ">>> >>> OrbiterRenderer_PocketFrog::StylusUp(%d, %d, button: %d)",
		p.x, p.y, aMouseButton);
#endif

	OrbiterLogic()->RegionUp(p.x, p.y);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::StylusMove( Point p )
{
	Orbiter::Event orbiterEvent;
	orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
	orbiterEvent.data.region.m_iX = p.x;
	orbiterEvent.data.region.m_iY = p.y;

	OrbiterLogic()->ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::StylusWheel( Point p, int delta)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS, ">>> >>> OrbiterRenderer_PocketFrog::StylusWheel(%d, %d, delta: %d)",
		p.x, p.y, delta);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_PocketFrog::PocketFrogButtonDown(int button)
{
	HandleKeyEvents(WM_KEYDOWN, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_PocketFrog::PocketFrogButtonUp(int button)
{
	HandleKeyEvents(WM_KEYUP, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Orbiter::Event orbiterEvent;
    orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
    orbiterEvent.data.button.m_iPK_Button = 0;

    if(uMsg == WM_KEYDOWN)
	{
        orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Key down %d bControlDown %d bAltDown %d bCapsLock %d bShiftDown %d",
			wParam,(int) OrbiterLogic()->m_bControlDown,(int) OrbiterLogic()->m_bAltDown,(int) OrbiterLogic()->m_bCapsLock,(int) OrbiterLogic()->m_bShiftDown);
#endif
	}
    else if(uMsg == WM_KEYUP)
        orbiterEvent.type = Orbiter::Event::BUTTON_UP;

    if(!TranslateVirtualKeys2PlutoKeys(uMsg, wParam, lParam, orbiterEvent.data.button.m_iPK_Button, 
        OrbiterLogic()->m_bShiftDown, OrbiterLogic()->m_bAltDown, OrbiterLogic()->m_bControlDown, OrbiterLogic()->m_bCapsLock)
    )
        return; //prevent auto-repeted

    //orbiter win xp/ce specifics
    if( wParam == VK_F12 && orbiterEvent.type == Orbiter::Event::BUTTON_UP) 
    {
		OnQuit();
    }
#ifdef WINCE
    else if( wParam == VK_F10) 
#else
    else if( wParam == VK_A && OrbiterLogic()->m_bControlDown)
#endif
    {
        ShowMainDialog();
        OrbiterLogic()->m_bControlDown = false;
    }
    else if( wParam == VK_D && OrbiterLogic()->m_bShiftDown && OrbiterLogic()->m_bControlDown)
    {
        DumpLocks();
    }
    else if( wParam == VK_P && OrbiterLogic()->m_bControlDown && uMsg == WM_KEYUP)
    {
        static int Count = 0;
        OrbiterLogic()->m_bControlDown = false;

#ifdef WINCE
        wchar_t wPath[4096];

        string sPath = string("/backbuffer") + StringUtils::ltos(Count++);
        mbstowcs(wPath, sPath.c_str(), 4096);	
        SaveImage(GetDisplay()->GetBackBuffer(), wPath);
#else
        SaveImage(GetDisplay()->GetBackBuffer(), (string("backbuffer") + StringUtils::ltos(Count++)).c_str());
#endif
    }

    if(wParam == VK_SHIFT)
        OrbiterLogic()->m_bShiftDownOnScreenKeyboard = false;

	RecordKeyboardAction(orbiterEvent.data.button.m_iPK_Button);

    OrbiterLogic()->ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ Pixel OrbiterRenderer_PocketFrog::GetColor16(PlutoColor color)
{
	//return (Pixel)(((color.R() << RED_SHIFT) & RED_MASK) | ((color.G() << 3) & GREEN_MASK) | (color.B() >> 3));		
    return Color(color.R(), color.G(), color.B());
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE OrbiterRenderer_PocketFrog::GetRedColor(Pixel pixel)
{
	return (pixel & RED_MASK)   >> (RED_SHIFT - (8 - RED_BITS));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE OrbiterRenderer_PocketFrog::GetGreenColor(Pixel pixel)
{
	return (pixel & GREEN_MASK) >> (GREEN_SHIFT - (8 - GREEN_BITS));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE OrbiterRenderer_PocketFrog::GetBlueColor(Pixel pixel)
{
	return (pixel & BLUE_MASK)  << (8 - BLUE_BITS); 
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	Rect srcRect;
	srcRect.Set(point.X + pObj->m_rPosition.Left(), point.Y + pObj->m_rPosition.Top(), 
		point.X + pObj->m_rPosition.Right() + 1, point.Y + pObj->m_rPosition.Bottom() + 1);

    if(srcRect.right >= OrbiterLogic()->m_iImageWidth)
        srcRect.right = OrbiterLogic()->m_iImageWidth - 1;

    if(srcRect.bottom >= OrbiterLogic()->m_iImageHeight)
        srcRect.bottom = OrbiterLogic()->m_iImageHeight - 1;
                
	Surface *pSurface = GetDisplay()->CreateSurface(pObj->m_rPosition.Width, pObj->m_rPosition.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
    delete pRasterizer;

	if(pObj->m_pGraphicToUndoSelect)
		pObj->m_pGraphicToUndoSelect->Clear();

	pObj->m_pGraphicToUndoSelect = new PocketFrogGraphic(pSurface);
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_PocketFrog::GetBackground( PlutoRectangle &rect )
{
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);

	ClipRectangle(rect);

	Rect srcRect;
	srcRect.Set(rect.X, rect.Y, rect.Right() + 1, rect.Bottom() + 1);

	Surface *pSurface = GetDisplay()->CreateSurface(rect.Width, rect.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
    delete pRasterizer;
	
	return new PocketFrogGraphic(pSurface);
}
//-----------------------------------------------------------------------------------------------------
PlutoGraphic *OrbiterRenderer_PocketFrog::CreateGraphic()
{
	return new PocketFrogGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::RenderScreen( bool bRenderGraphicsOnly )
{
	if( bRenderGraphicsOnly )
	{
		OrbiterRenderer::RenderScreen( bRenderGraphicsOnly );
		UpdateScreen();
		return;
	}

	CHECK_STATUS();
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",
		(OrbiterLogic()->m_pScreenHistory_Current ? OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
#endif
	
	if (EnableOpenGL)
	{
#ifndef WINCE
		Rect srcRect;
		srcRect.Set(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);
		auto_ptr<Surface> spSurface(GetDisplay()->CreateSurface(OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight));
		auto_ptr<Rasterizer> spRasterizer(GetDisplay()->CreateRasterizer(spSurface.get()));
		spRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
		m_spBeforeGraphic.reset(new PocketFrogGraphic(spSurface.get()));
		spRasterizer.release();
		spSurface.release();
#endif
	}

	if (OrbiterLogic()->m_pScreenHistory_Current)
	{
		PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
		GetDisplay()->FillRect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight, 0x0000);
	}

	m_bPoolRendering=true;
	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);
	m_bPoolRendering=false;

	if( m_vectPooledTextToRender.size())
	{
        RECT rectLocation;
		HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);
		for(size_t s=0;s<m_vectPooledTextToRender.size();++s)
		{
			TextToRenderInfo *pTextToRenderInfo = m_vectPooledTextToRender[s];
			RenderText(hdc,pTextToRenderInfo->TextToDisplay,pTextToRenderInfo->rPosition,pTextToRenderInfo->iPK_HorizAlignment,pTextToRenderInfo->iPK_VertAlignment,
				pTextToRenderInfo->sFont,pTextToRenderInfo->ForeColor,pTextToRenderInfo->iPixelHeight,pTextToRenderInfo->bBold,pTextToRenderInfo->bItalic,pTextToRenderInfo->bUnderline,
				pTextToRenderInfo->point, rectLocation);
			delete pTextToRenderInfo;
		}
		GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
		m_vectPooledTextToRender.clear();
	}

	if (EnableOpenGL)
	{
#ifndef WINCE
		if(m_spAfterGraphic.get())
			m_spAfterGraphic->Initialize();

		m_spAfterGraphic.reset(new PocketFrogGraphic(GetDisplay()->GetBackBuffer()));

		if(OrbiterLogic()->m_pObj_SelectedLastScreen)
		{
			m_rectLastSelected = PlutoRectangle(OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition);
			m_rectLastSelected.Y = OrbiterLogic()->m_iImageHeight - OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition.Y;
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "No object selected? :o");
			m_rectLastSelected.X = 0;
			m_rectLastSelected.Y = OrbiterLogic()->m_iImageHeight;
			m_rectLastSelected.Width = 80;
			m_rectLastSelected.Height = 80;
		}

#ifndef DISABLE_OPENGL
		if(m_Desktop && m_Desktop->EffectBuilder)
		{
			m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());

			//temp for the show. remove me!
			if(OrbiterLogic()->UsesUIVersion2() && NULL != m_Desktop && NULL != m_Desktop->EffectBuilder)
			{
				//TODO: this is temporary
				int nCurrentDesignObjID = OrbiterLogic()->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
				PlutoRectangle rectStartEffect(0, 0, OrbiterLogic()->m_iImageWidth, OrbiterLogic()->m_iImageHeight);
				int nPK_Effect = 0;
				int nTransitionTimeIsMs = Simulator::GetInstance()->m_iMilisecondsTransition; 
				switch(nCurrentDesignObjID) 
				{
				case DESIGNOBJ_mnuMenu2_CONST:
					nPK_Effect = EFFECT_Basic_transit_effect_CONST;
					break;

				case DESIGNOBJ_mnuAmbiance_CONST:
					nTransitionTimeIsMs = 1000;
					rectStartEffect.Location(PlutoPoint(0, OrbiterLogic()->m_iImageHeight - 40));
					rectStartEffect.Size(PlutoSize(100, 40));
					nPK_Effect = EFFECT_Bezier_transit_prism_CONST;
					break;

				case DESIGNOBJ_mnuSpeedControl_CONST:
					nTransitionTimeIsMs = 1000;
					rectStartEffect.Location(PlutoPoint(0, OrbiterLogic()->m_iImageHeight - 40));
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

				if(!Transit)
					Transit = m_Desktop->EffectBuilder->
					CreateEffect(
					GL2D_EFFECT_TRANSIT_NO_EFFECT,
					Simulator::GetInstance()->m_iMilisecondsTransition
					);

				StartAnimation();
				return;
			}

			if(OrbiterLogic()->m_pObj_SelectedLastScreen)
			{
				m_rectLastSelected = PlutoRectangle(OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition);
				m_rectLastSelected.Y = OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition.Y;

#ifdef DEBUG
				g_pPlutoLogger->Write(LV_WARNING, "Effect with change: %d", OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange);
#endif

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
					Transit->Configure(&m_rectLastSelected);
			}
			else
			{
				GL2DEffect* Transit = m_Desktop->EffectBuilder->
					CreateEffect(
					GL2D_EFFECT_TRANSIT_NO_EFFECT,
					400
					);
			}
		}
#endif

		StartAnimation();
#endif
	}
	else
	{
		UpdateScreen();
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
    bool bDisableAspectRatio, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtPocketFrogGraphic)
		return;//nothing to render or not an pocket frog graphic

	if(pPlutoGraphic->IsEmpty())
		return;//nothing to render or not an pocket frog graphic

	PocketFrogGraphic *pPocketFrogGraphic = (PocketFrogGraphic *) pPlutoGraphic;
	Surface *pSurface = pPocketFrogGraphic->m_pSurface;

    rectTotal.X += point.X;
    rectTotal.Y += point.Y;

	if(pSurface->GetWidth() == 0 || pSurface->GetHeight() == 0)
    {
        //do nothing
    }
	else
		if(pSurface->GetWidth() != rectTotal.Width || pSurface->GetHeight() != rectTotal.Height)
		{
			Rect dest;	
			dest.Set(rectTotal.Left(), rectTotal.Top(), rectTotal.Right(), rectTotal.Bottom());

			double ZoomX = 1;
			double ZoomY = 1;

			if(bDisableAspectRatio) //no aspect ratio kept
			{
				ZoomX = rectTotal.Width / double(pSurface->GetWidth());
				ZoomY = rectTotal.Height / double(pSurface->GetHeight());
			}
			else //we'll have to keep the aspect
			{
				ZoomX = ZoomY = min(rectTotal.Width / double(pSurface->GetWidth()),
					rectTotal.Height / double(pSurface->GetHeight()));
			}

			dest.right = dest.left + int(pSurface->GetWidth() * ZoomX);
			dest.bottom = dest.top + int(pSurface->GetHeight() * ZoomY);

			if( dest.right-dest.left>0 && dest.bottom-dest.top>0 )  // PF crashes with 0 width/height
				GetDisplay()->BlitStretch(dest, pSurface); 
		}
		else
		{
			GetDisplay()->Blit( rectTotal.X, rectTotal.Y, pSurface );
		}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::EndPaint()
{
	if(EnableOpenGL)
	{
#ifndef DISABLE_OPENGL
		//the surface after the screen was rendered
		if(m_spAfterGraphic.get())
			m_spAfterGraphic->Initialize();

		m_spAfterGraphic.reset(new PocketFrogGraphic(GetDisplay()->GetBackBuffer()));

		m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
		GL2DEffect * Effect = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_NOEFFECT, 
			1);
		StartAnimation();
#endif //opengl stuff
	}

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_WARNING, "End paint...");
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
	PlutoRectangle localrect = rect;
	localrect.X += point.X;
	localrect.Y += point.Y;

	ClipRectangle(localrect);

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);

	Rect rectUpdate;
	rectUpdate.Set(localrect.Left(), localrect.Top(), localrect.Right(), localrect.Bottom());

	if(!EnableOpenGL)
		GetDisplay()->Update(&rectUpdate);
	else
		UpdateScreen();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::OnQuit()
{
	pthread_cond_broadcast( &OrbiterLogic()->m_listMessageQueueCond );
	::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::WriteStatusOutput(const char* pMessage)
{
	//
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::TryToUpdate()
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	UpdateScreen();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::UpdateScreen()
{
	if (!EnableOpenGL)
	{
		GetDisplay()->Update();
	}
	else
	{ 
#ifndef WINCE
		if(m_Desktop && m_Desktop->EffectBuilder && 
			!m_Desktop->EffectBuilder->HasEffects())
		{
			if(m_spAfterGraphic.get())
				m_spAfterGraphic->Initialize();

			m_spAfterGraphic.reset(new PocketFrogGraphic(GetDisplay()->GetBackBuffer()));

			m_Desktop->EffectBuilder->Widgets->ConfigureNextScreen(m_spAfterGraphic.get());
			GL2DEffect* Transit = m_Desktop->EffectBuilder->CreateEffect(GL2D_EFFECT_NOEFFECT, 400);
			StartAnimation();
		}
#endif
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::GameSuspend()
{
	Game::GameSuspend();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::GameResume()
{
	Game::GameResume();
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_PocketFrog::IsFullScreen() 
{ 
	return OrbiterLogic()->m_bFullScreen; 
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_PocketFrog::GetWindowWidth() { return OrbiterLogic()->m_iImageWidth;  } 
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_PocketFrog::GetWindowHeight() { return OrbiterLogic()->m_iImageHeight; } 
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::PingFailed()
{
	g_pPlutoLogger->Write(LV_CRITICAL, "Ping Failed! About to reload .. ");

	ShowMainDialog();

	OrbiterLogic()->m_bQuit = true;
	OrbiterLogic()->m_bReload = true;
	OnQuit();
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::AdjustWindowSize(int iOrbiterWidth, int iOrbiterHeight)
{
#ifndef WINCE
    iOrbiterWidth += 6; //the border
    iOrbiterHeight += 25; //the title + the border

    RECT rc;
    HWND hWndDesktop = ::GetDesktopWindow();
    ::GetWindowRect(hWndDesktop, &rc);

    int iDesktopWidth = rc.right - rc.left;
    int iDesktopHeight = rc.bottom - rc.top;

    rc.left = (iDesktopWidth - iOrbiterWidth) / 2;
    rc.right = rc.left + iOrbiterWidth;
    rc.top = (iDesktopHeight - iOrbiterHeight) / 2;
    rc.bottom = rc.top + iOrbiterHeight;
    MoveWindow(rc.left, rc.top, iOrbiterWidth, iOrbiterHeight, TRUE);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment)
{
	if( OrbiterLogic()->m_iRotation==0 ) 
	{
		rectLocation.left = rPosition.Left();
		rectLocation.right = rPosition.Right();

		if( iVertAlignment==VERTALIGNMENT_Bottom_CONST )
			rectLocation.top = rPosition.Bottom() - iHeight;
		else if( iVertAlignment==VERTALIGNMENT_Middle_CONST )
			rectLocation.top = rPosition.Top() + ((rPosition.Height - iHeight)/2);
		else
			rectLocation.top = rPosition.Top();

		rectLocation.bottom = rectLocation.top+iHeight;
	}
	else if( OrbiterLogic()->m_iRotation==180 )
	{
		rectLocation.left = 0;
		rectLocation.right = rPosition.Width;

		if( iVertAlignment==VERTALIGNMENT_Bottom_CONST )
			rectLocation.top = rPosition.Height - iHeight;
		else if( iVertAlignment==VERTALIGNMENT_Middle_CONST )
			rectLocation.top = ((rPosition.Height - iHeight)/2);
		else
			rectLocation.top = 0;

		rectLocation.bottom = rectLocation.top+iHeight;
	}
	else if( OrbiterLogic()->m_iRotation==90 || OrbiterLogic()->m_iRotation==270 )
	{
		rectLocation.left = 0;
		rectLocation.right = rPosition.Height;

		if( iVertAlignment==VERTALIGNMENT_Bottom_CONST )
			rectLocation.top = rPosition.Width - iHeight;
		else if( iVertAlignment==VERTALIGNMENT_Middle_CONST )
			rectLocation.top = ((rPosition.Width - iHeight)/2);
		else
			rectLocation.top = 0;

		rectLocation.bottom = rectLocation.top+iHeight;
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::OnUnexpectedDisconnect()
{
    //restarting orbiter
    PROCESS_INFORMATION pi;
    ::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    STARTUPINFO si;
    ::ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = 0;

    string sCmdLine;

#ifdef WINCE
    wchar_t pProcessNameW[256];
    ::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));

    sCmdLine += "-d " + StringUtils::ltos(OrbiterLogic()->m_dwPK_Device);
    sCmdLine += " -r " + CmdLineParams.sRouter_IP;
    sCmdLine += " -l \"" + CmdLineParams.sLogger + "\"";

    if(CmdLineParams.bFullScreen)
        sCmdLine += " -F";

    wchar_t CmdLineW[256];
    mbstowcs(CmdLineW, sCmdLine.c_str(), 256);

    ::CreateProcess(pProcessNameW, CmdLineW, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
#else
    sCmdLine = GetCommandLine();
    ::CreateProcess(NULL, const_cast<char *>(sCmdLine.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
#endif

    OrbiterLogic()->m_bQuit = true;
    exit(1); //die!!!
}
//-----------------------------------------------------------------------------------------------------
int OrbiterRenderer_PocketFrog::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
    return PromptUserEx(sPrompt, p_mapPrompts);
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_PocketFrog::DisplayProgress(string sMessage, int nProgress)
{
    return DialogProgressEx(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::DumpLocks() 
{
    string sMessage = 
        FormatMutexMessage(OrbiterLogic()->m_ScreenMutex) +
        FormatMutexMessage(OrbiterLogic()->m_VariableMutex) +
        FormatMutexMessage(OrbiterLogic()->m_DatagridMutex) +
        FormatMutexMessage(OrbiterLogic()->m_MaintThreadMutex) +
        FormatMutexMessage(OrbiterLogic()->m_NeedRedrawVarMutex);

#ifdef WINCE
    wchar_t MessageW[4096];
    mbstowcs(MessageW, sMessage.c_str(), 4096);
    ::MessageBox(m_hWnd, MessageW, TEXT("Orbiter mutexes"), MB_OK);
#else
    ::MessageBox(m_hWnd, sMessage.c_str(), TEXT("Orbiter mutexes"), MB_OK);
#endif
}
//-----------------------------------------------------------------------------------------------------
string OrbiterRenderer_PocketFrog::FormatMutexMessage(pluto_pthread_mutex_t& PlutoMutex)
{
    string sMessage =
        "Mutex '" + PlutoMutex.m_sName + "':\r\n" + 
        "    - " + StringUtils::ltos(PlutoMutex.m_NumLocks) + " lock(s)" + "\r\n" + 
        "    - " + PlutoMutex.m_sFileName + ":" + StringUtils::ltos(PlutoMutex.m_Line) + "\r\n";

    return sMessage;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::RenderFrame(void *data)
{
#ifndef WINCE
	if(m_Desktop && m_Desktop->EffectBuilder && (m_Desktop->EffectBuilder->HasEffects()))
	{
		OrbiterLogic()->CallMaintenanceInMiliseconds(0, (OrbiterCallBack) &Orbiter::RenderFrame, 
			NULL, pe_NO);
		OnIdle();
	}
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::StartAnimation()
{
	m_bPaintDesktop = false;
	OrbiterLogic()->CallMaintenanceInMiliseconds(0, (OrbiterCallBack) &Orbiter::RenderFrame, 
		NULL, pe_NO);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::OnIdle()
{
#ifndef DISABLE_OPENGL
	if(EnableOpenGL)
	{
		Sleep(5);
		WakeupFromCondWait();
	}
#endif
}

//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::WakeupFromCondWait()
{
#ifndef DISABLE_OPENGL
	pthread_cond_broadcast(&m_GLThreadCond);
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::SelectObject( class DesignObj_Orbiter *pObj, PlutoPoint point )
{
	if (!EnableOpenGL) 
	{
		OrbiterLogic()->SelectObject(pObj, point);
		return;
	}

	if(EnableOpenGL)
	{
#ifndef DISABLE_OPENGL
		if (!OrbiterLogic()->m_pObj_SelectedLastScreen)
			Commons3D::Instance().SetSelectedArea(NULL);


		FloatRect SelectedArea;
		SelectedArea.Left   = (float)point.X + pObj->m_rBackgroundPosition.X;
		SelectedArea.Top    = (float)(point.Y + pObj->m_rBackgroundPosition.Y);
		SelectedArea.Width  = (float)pObj->m_rBackgroundPosition.Width;
		SelectedArea.Height = (float)pObj->m_rBackgroundPosition.Height;

		Commons3D::Instance().SetSelectedArea(&SelectedArea);

		GL2DEffect* Effect = NULL;

		Effect = m_Desktop->EffectBuilder->
			CreateEffect(
				m_Desktop->EffectBuilder->GetEffectCode(OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_NoChange),
				Simulator::GetInstance()->m_iMilisecondsHighLight
			);
		if(!Effect)
			Effect = m_Desktop->EffectBuilder->
			CreateEffect(
				GL2D_EFFECT_HIGHLIGHT_AREA,
				Simulator::GetInstance()->m_iMilisecondsHighLight
			);

		if(!Effect)
			return;

		StartAnimation();

		PlutoRectangle SeclectedAreaEffectSize;
		SeclectedAreaEffectSize.X = point.X + pObj->m_rBackgroundPosition.X;
		SeclectedAreaEffectSize.Y = point.Y + pObj->m_rBackgroundPosition.Y;
		SeclectedAreaEffectSize.Width = pObj->m_rBackgroundPosition.Width;
		SeclectedAreaEffectSize.Height = pObj->m_rBackgroundPosition.Height;
		
		Effect->Configure(&SeclectedAreaEffectSize);
#endif
	}
}  
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::DoHighlightObject()
{
	if(!EnableOpenGL)
	{
		OrbiterRenderer::DoHighlightObject();
	}
	else
	{
#ifndef DISABLE_OPENGL
		DoHighlightObjectOpenGL();
#endif
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::DoHighlightObjectOpenGL()
{
#ifndef DISABLE_OPENGL
	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour)
	{
		Commons3D::Instance().SetHighLightArea(NULL);
		return;
	}

	PLUTO_SAFETY_LOCK( cm, OrbiterLogic()->m_ScreenMutex );  // Protect the highlighed object
	if( OrbiterLogic()->m_pGraphicBeforeHighlight )
		UnHighlightObject();

	if( !OrbiterLogic()->m_pObj_Highlighted )
	{
		Commons3D::Instance().SetHighLightArea(NULL);
		return;
	}

	if( OrbiterLogic()->m_pObj_Highlighted->m_ObjectType==DESIGNOBJTYPE_Datagrid_CONST )
	{
		DesignObj_DataGrid *pGrid = (DesignObj_DataGrid *) OrbiterLogic()->m_pObj_Highlighted;
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
		OrbiterLogic()->GetGridCellDimensions( pGrid,  
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

	//m_pGraphicBeforeHighlight = GetBackground(m_rectLastHighlight);
	FloatRect HighLightArea;
	HighLightArea.Left = (float)OrbiterLogic()->m_rectLastHighlight.Left();
	HighLightArea.Top = (float)OrbiterLogic()->m_rectLastHighlight.Top();
	HighLightArea.Width =  (float)OrbiterLogic()->m_rectLastHighlight.Width;
	HighLightArea.Height = (float)OrbiterLogic()->m_rectLastHighlight.Height;

	Commons3D::Instance().SetHighLightArea(&HighLightArea);
	GL2DEffect* Effect = NULL;

	if (OrbiterLogic()->m_pObj_SelectedLastScreen)
	 Effect = m_Desktop->EffectBuilder->
		CreateEffect(
			m_Desktop->EffectBuilder->GetEffectCode(OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Highlighted),
			Simulator::GetInstance()->m_iMilisecondsHighLight
		);
	if(!Effect)
		Effect = m_Desktop->EffectBuilder->
			CreateEffect(
				GL2D_EFFECT_HIGHLIGHT_AREA,
				Simulator::GetInstance()->m_iMilisecondsHighLight
			);
	if(Effect)
		Effect->Configure(&OrbiterLogic()->m_rectLastHighlight);

	StartAnimation();
#endif
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::GetWindowPosition(PlutoPoint& point)
{
	RECT rect;
	GetWindowRect(&rect);

	point.X = rect.left + BORDER_WIDTH;
	point.Y = rect.top + BORDER_WIDTH + TITLEBAR_HEIGHT;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::EventLoop()
{
	this->Run();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_PocketFrog::ReplaceColorInRectangle(int x, int y, int width, int height, 
	PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
	ClipRectangle(x, y, width, height);

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	Pixel pixelSrc = GetColor16(ColorToReplace);
	Pixel pixelDest = GetColor16(ReplacementColor);
	Pixel pixelCurrent;

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			if(i + x >= 0 && j + y >= 0 && i + x < OrbiterLogic()->m_iImageWidth && j + y < OrbiterLogic()->m_iImageHeight)
			{
				pixelCurrent = GetDisplay()->GetPixel(i + x, j + y);

				if(pixelSrc == pixelCurrent)
					GetDisplay()->SetPixel(i + x, j + y, pixelDest);
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::RenderText(string &TextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point)
{
    CHECK_STATUS();
    PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);

	if(!m_bPoolRendering || TextToDisplay.find("~S") != string::npos)
	{
        RECT rectLocation;
		HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);

        //handle escape sequences
        if(TextToDisplay.find("~S") != string::npos)
        {   
            string sText = TextToDisplay;
            vector<string> vectTextPieces;
            size_t nPos = 0;
            bool bMultiLine = TextToDisplay.find("\n") != string::npos;
            while((nPos = sText.find("~S")) != string::npos)
            {
                size_t nNextPos = nPos;
                if(nPos == 0)
                {
                    nNextPos = sText.find("~S", nPos + 1);

                    if(nNextPos == string::npos)
                        nNextPos = sText.length();

                    if(sText.find("~", nNextPos + 1) == string::npos)
                        nNextPos = sText.length();
                }

                string sTextPiece = sText.substr(0, nNextPos);
                vectTextPieces.push_back(sTextPiece);
                sText = sText.substr(nNextPos);
            }

            PlutoRectangle plutoRect = Text->m_rPosition;

            vector<string>::iterator it;
            for(it = vectTextPieces.begin(); it != vectTextPieces.end(); it++)
            {
                string sTextPiece = *it;
                string sTextToRender = sTextPiece;
                size_t nPos = sTextPiece.find("~S");
                TextStyle *pPieceTextStyle = pTextStyle;
                if(nPos == 0)
                {
                    int nNextPos = int(sTextPiece.find("~", nPos + 1));
                    string sTextStyleNumber = sTextPiece.substr(nPos + 2, nNextPos - 2);
                    int nTextStyle = atoi(sTextStyleNumber.c_str());
                    pPieceTextStyle = OrbiterLogic()->m_mapTextStyle_Find(nTextStyle);

                    sTextToRender = sTextPiece.substr(nNextPos + 1);

                    if(!pPieceTextStyle)
                    {
                        g_pPlutoLogger->Write(LV_CRITICAL, "Text style not found %d. Using default!", nTextStyle);
                        pPieceTextStyle = pTextStyle;
                    }
                }

                RenderText(hdc,sTextToRender,Text->m_rPosition,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment,
                    pPieceTextStyle->m_sFont,pPieceTextStyle->m_ForeColor,pPieceTextStyle->m_iPixelHeight,pPieceTextStyle->m_bBold,pPieceTextStyle->m_bItalic,pPieceTextStyle->m_bUnderline,
                    point, rectLocation);

                if(bMultiLine)
                    Text->m_rPosition.Y += rectLocation.bottom - rectLocation.top;
                else
                    Text->m_rPosition.X += rectLocation.right - rectLocation.left; 
            }
            
            Text->m_rPosition = plutoRect;
        }
        else //normal rendering
        {
		    RenderText(hdc,TextToDisplay,Text->m_rPosition,Text->m_iPK_HorizAlignment,Text->m_iPK_VertAlignment,
			    pTextStyle->m_sFont,pTextStyle->m_ForeColor,pTextStyle->m_iPixelHeight,pTextStyle->m_bBold,pTextStyle->m_bItalic,pTextStyle->m_bUnderline,
			    point, rectLocation);
        }

		GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
	}
	else
	{
		TextToRenderInfo *pTextToRenderInfo = new TextToRenderInfo;
		pTextToRenderInfo->TextToDisplay = TextToDisplay;
		pTextToRenderInfo->rPosition = PlutoRectangle(Text->m_rPosition);
		pTextToRenderInfo->iPK_HorizAlignment = Text->m_iPK_HorizAlignment;
		pTextToRenderInfo->iPK_VertAlignment = Text->m_iPK_VertAlignment;
		pTextToRenderInfo->sFont = pTextStyle->m_sFont;
		pTextToRenderInfo->ForeColor = PlutoColor(pTextStyle->m_ForeColor);
		pTextToRenderInfo->iPixelHeight = pTextStyle->m_iPixelHeight;
		pTextToRenderInfo->bBold = pTextStyle->m_bBold;
		pTextToRenderInfo->bItalic = pTextStyle->m_bItalic;
		pTextToRenderInfo->bUnderline = pTextStyle->m_bUnderline;
		pTextToRenderInfo->point = PlutoPoint(point);
		m_vectPooledTextToRender.push_back(pTextToRenderInfo);
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::RenderText(HDC hdc,string &TextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectCalcLocation)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	int iRotation = OrbiterLogic()->m_iRotation;

    RECT rectLocation;
	rectLocation.left   = 0;
	rectLocation.top    = 0;

	HDC hdc_drawing=0;

	if( iRotation==0 )
	{
		hdc_drawing=hdc;
		rectLocation.right  = rPosition.Width;
		rectLocation.bottom = rPosition.Height;
	}
	else
	{
		if( iRotation==180 )
		{
			rectLocation.right  = rPosition.Width;
			rectLocation.bottom = rPosition.Height;
		}
		else
		{
			rectLocation.right  = rPosition.Height;
			rectLocation.bottom = rPosition.Width;
		}

		hdc_drawing = CreateCompatibleDC(GetDisplay()->GetBackBuffer()->GetDC(false));
		// Create a bitmap big enough for our client rectangle.
		HBITMAP hbmMem = ::CreateCompatibleBitmap(hdc,
										rectLocation.right,
										rectLocation.bottom);

		HGDIOBJ  hbmOld = ::SelectObject(hdc_drawing, hbmMem);
		::DeleteObject(hbmOld);

		HBRUSH hbr = ::CreateSolidBrush(RGB(128,0,0));
		HBRUSH hbrOld = (HBRUSH) ::SelectObject(hdc_drawing,hbr);
		::FillRect(hdc_drawing,&rectLocation,hbr);
		::SelectObject(hdc_drawing,hbrOld);
		::DeleteObject(hbr);
	}
	
	LOGFONT lf;
	HFONT hFontNew, hFontOld;

	// Clear out the lf structure to use when creating the font.
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight		= iPixelHeight * MATCH_SDL_FONT_HEIGHT;

	//adjust minumum text height 
	if(lf.lfHeight < 12)
		lf.lfHeight = 12;

	if( iRotation==0 )
		lf.lfQuality	= DRAFT_QUALITY;
	else
		lf.lfQuality	= DEFAULT_QUALITY;
	lf.lfWeight		= bBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic		= bItalic;
	lf.lfUnderline	= bUnderline;
/*	MS' Gdi doesn't seem to work, we have to hack in a work around
	if( m_iRotation )
	{
		//use this to rotate text
		lf.lfOrientation = (360 - m_iRotation) * 10;
		lf.lfEscapement = (360 - m_iRotation) * 10;
	}
*/
#ifdef WINCE
    wchar_t wFaceName[1024];
    mbstowcs(wFaceName, sFont.c_str(), 1024);	
	wcscpy(lf.lfFaceName, wFaceName);
#else
	strcpy(lf.lfFaceName, sFont.c_str());
#endif

	hFontNew = ::CreateFontIndirect(&lf);
	hFontOld = (HFONT) ::SelectObject(hdc_drawing, hFontNew);
	

#ifdef WINCE
    wchar_t wText[1024];
    mbstowcs(wText, TextToDisplay.c_str(), 1024);
#endif

#ifndef WINCE
	#define TEXT_TO_RENDER TextToDisplay.c_str()
#else
	#define TEXT_TO_RENDER wText
#endif

	//calculate rect first
	::DrawText(hdc_drawing, TEXT_TO_RENDER, int(TextToDisplay.length()), &rectLocation, 
		DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT | DT_MODIFYSTRING | DT_END_ELLIPSIS ); 

	int iRealHeight = rectLocation.bottom;
	if( iRealHeight>rPosition.Height )
		iRealHeight=rPosition.Height; // Crop to the area

    rectCalcLocation = rectLocation;

	// Figure out where to put this
	CalcTextRectangle(rectLocation,rPosition,iRotation,iRealHeight,iPK_VertAlignment);

	if( iRotation==0 )
	{
		rectLocation.left += point.X;
		rectLocation.right += point.X;
		rectLocation.top += point.Y;
		rectLocation.bottom += point.Y;
	}

 	::SetTextColor(hdc_drawing, RGB(ForeColor.R(), ForeColor.G(), ForeColor.B()));
	::SetBkMode(hdc_drawing, TRANSPARENT);

	switch (iPK_HorizAlignment)
	{
		case HORIZALIGNMENT_Left_CONST: 
			::DrawText(hdc_drawing, TEXT_TO_RENDER, int(TextToDisplay.length()), &rectLocation, 
				DT_WORDBREAK | DT_NOPREFIX | DT_MODIFYSTRING | DT_END_ELLIPSIS); 
		break;

        case HORIZALIGNMENT_Center_CONST: 
            ::DrawText(hdc_drawing, TEXT_TO_RENDER, int(TextToDisplay.length()), &rectLocation, 
                DT_WORDBREAK | DT_CENTER | DT_NOPREFIX | DT_MODIFYSTRING | DT_END_ELLIPSIS); 
        break;

        default: 
            ::DrawText(hdc_drawing, TEXT_TO_RENDER, int(TextToDisplay.length()), &rectLocation, 
				DT_WORDBREAK | DT_CENTER | DT_NOPREFIX | DT_MODIFYSTRING | DT_END_ELLIPSIS); 
            break;
	}

	::SelectObject(hdc_drawing, hFontOld);
	::DeleteObject(hFontNew);

	if( hdc_drawing!=hdc )
	{
		for(int x=0;x<(iRotation==180 ? rPosition.Width : rPosition.Height);x++)
		{
			for(int y=0;y<(iRotation==180 ? rPosition.Height: rPosition.Width);y++)
			{
				COLORREF c = ::GetPixel(hdc_drawing,x,y);
				if( c==132 || c==128 )
					continue;  // Unchanged
				if( iRotation==90 )
					::SetPixel(hdc,rPosition.Right()-y,rPosition.Y+x,c);
				else if( iRotation==180 )
					::SetPixel(hdc,rPosition.Right()-x,rPosition.Bottom()-y,c);
				else if( iRotation==270 )
					::SetPixel(hdc,rPosition.X+y,rPosition.Bottom()-x,c);
			}
		}
		DeleteDC(hdc_drawing);
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::BatchedTextRendering(bool bValue)
{
	m_bPoolRendering = bValue;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::SolidRectangle(int x, int y, int width, int height, PlutoColor color)
{
	ClipRectangle(x, y, width, height);

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	GetDisplay()->FillRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_PocketFrog::HollowRectangle(int x, int y, int width, int height, PlutoColor color)
{
	ClipRectangle(x, y, width, height);

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, OrbiterLogic()->m_ScreenMutex);
	GetDisplay()->DrawRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
