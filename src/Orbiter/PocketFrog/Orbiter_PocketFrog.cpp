#include "Orbiter_PocketFrog.h"

#include "Wingdi.h"
#include "MainDialog.h"
#include "Resource.h"
#include "SelfUpdate.h"
#include "VirtualKeysTranslator.h"
#include "PopupMessage.h"
#include "ProgressDialog.h"
#include "ScreenHistory.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 
#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 

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
//-----------------------------------------------------------------------------------------------------
//progress bar custom settings
#define PROGRESS_MAX 100
#define PROGRESS_OPTIMAL_SIZE 300
#define PROGRESS_UPDATE_STEP 10
#define PROGRESS_BOTTOM_DISTANCE 50
#define PROGRESS_HEIGHT 20

namespace ProgressBar
{
	PlutoColor TextColor(0xDD, 0xDD, 0xDD);
	PlutoColor TextShadowColor = PlutoColor::Black();
	PlutoColor BorderColor(0xB5, 0xB5, 0xB5);
	PlutoColor FillBackgroundColor(0x66, 0x66, 0x66);
	PlutoColor ProgressStartColor(0x10, 0x76, 0xE4);
	PlutoColor ProgressEndColor(0x3D, 0xB2, 0x14);
};
using namespace ProgressBar;
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
#define CHECK_STATUS() { if(m_bQuit) return; }
//-----------------------------------------------------------------------------------------------------
Orbiter_PocketFrog *Orbiter_PocketFrog::m_pInstance = NULL; //the one and only
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
Orbiter_PocketFrog::Orbiter_PocketFrog(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, 
									   bool bLocalMode, int nImageWidth, int nImageHeight, 
									   bool bFullScreen, bool bUseOpenGL) :
		Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight)
{
    m_config.orientation      = ORIENTATION_WEST;
    m_config.splashScreenTime = 0;	
	m_bUpdating = false;
	m_bFullScreen=bFullScreen;
	m_bUseOpenGL = bUseOpenGL;

#ifdef WINCE
	m_bUseOpenGL = false; //opengl not available for WINCE
#endif

	m_bConnectionLost = false;
	m_bReload = false;
	m_bQuit = false;
	m_bWeCanRepeat = true;
	m_bPoolRendering = true;

	//used to render some text
	m_spTextStyle.reset(new TextStyle());
	m_spTextStyle->m_bBold = true;
	m_spTextStyle->m_bItalic = false;
	m_spTextStyle->m_bUnderline = false;
	m_spTextStyle->m_ForeColor = PlutoColor::Black();
	m_spTextStyle->m_sFont = "Arial";
	m_spTextStyle->m_iPixelHeight = 15;
	m_spTextStyle->m_iPK_VertAlignment = VERTALIGNMENT_Middle_CONST;
	m_spTextStyle->m_iPK_HorizAlignment = HORIZALIGNMENT_Center_CONST;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Orbiter_PocketFrog::~Orbiter_PocketFrog()
{
#ifndef WINCE
	if (m_bUseOpenGL && m_spAfterGraphic.get())
		m_spAfterGraphic->Initialize();
#endif
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::GameInit()
{
    ::SetWindowText(m_hWnd, TEXT("Pluto Orbiter"));

#ifndef WINCE
    ::SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX);
#endif
    
    PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

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
        if(pLogoSurface->m_height == m_iImageHeight && pLogoSurface->m_width == m_iImageWidth)
		    GetDisplay()->Blit( 0, 0, pLogoSurface );
        else //zoom
        {
            Rect dest;	
            dest.Set(0, 0, m_iImageWidth, m_iImageHeight);

            double ZoomX = m_iImageWidth / double(pLogoSurface->GetWidth());
            double ZoomY = m_iImageHeight / double(pLogoSurface->GetHeight());

            dest.right = dest.left + int(pLogoSurface->GetWidth() * ZoomX);
            dest.bottom = dest.top + int(pLogoSurface->GetHeight() * ZoomY);     
            
            if( dest.right-dest.left>0 && dest.bottom-dest.top>0 )  // PF crashes with 0 width/height
                GetDisplay()->BlitStretch(dest, pLogoSurface); 
        }
	}
	else
	{
		GetDisplay()->FillRect(0, 0, m_iImageWidth, m_iImageHeight, 0x00);
	}

	if(pLogoSurface)
		delete pLogoSurface;

	GetDisplay()->Update();
	Initialize(gtPocketFrogGraphic);

	if(m_bQuit || m_bReload)
    {
		//we were just loading and someone did a reload router.
		//we'll start again
		OnUnexpectedDisconnect();
    }

	if (!m_bLocalMode)
		CreateChildren();

#ifndef WINCE
	if(m_bFullScreen)
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
/*virtual*/ void Orbiter_PocketFrog::ShowWindowCustom()
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
/*virtual*/ void Orbiter_PocketFrog::GameEnd()
{
	
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::GameLoop()
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusDown( Point stylus )
{
	RegionDown(stylus.x, stylus.y);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusUp( Point stylus )
{
	RegionUp(stylus.x, stylus.y);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::StylusMove( Point stylus )
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::PocketFrogButtonDown(int button)
{
	HandleKeyEvents(WM_KEYDOWN, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::PocketFrogButtonUp(int button)
{
	HandleKeyEvents(WM_KEYUP, button, 0L);
	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Orbiter::Event orbiterEvent;
    orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
    orbiterEvent.data.button.m_iPK_Button = 0;

    if(uMsg == WM_KEYDOWN)
	{
        orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"Key down %d bControlDown %d bAltDown %d bCapsLock %d bShiftDown %d",
			wParam,(int) m_bControlDown,(int) m_bAltDown,(int) m_bCapsLock,(int) m_bShiftDown);
#endif
	}
    else if(uMsg == WM_KEYUP)
        orbiterEvent.type = Orbiter::Event::BUTTON_UP;

    if(!TranslateVirtualKeys2PlutoKeys(uMsg, wParam, lParam, orbiterEvent.data.button.m_iPK_Button, 
        m_bShiftDown, m_bAltDown, m_bControlDown, m_bCapsLock)
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
    else if( wParam == VK_A && m_bControlDown)
#endif
    {
        ShowMainDialog();
        m_bControlDown = false;
    }
    else if( wParam == VK_D && m_bShiftDown && m_bControlDown)
    {
        DumpLocks();
    }
	else if( wParam == VK_P && m_bShiftDown && m_bControlDown && uMsg == WM_KEYUP)
	{
#ifdef DEBUG
		DumpScreenHistory();
#endif
	}
    else if( wParam == VK_P && m_bControlDown && uMsg == WM_KEYUP)
    {
        static int Count = 0;
        m_bControlDown=false;

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
        m_bShiftDownOnScreenKeyboard = false;

    Orbiter::ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ Pixel Orbiter_PocketFrog::GetColor16(PlutoColor color)
{
	//return (Pixel)(((color.R() << RED_SHIFT) & RED_MASK) | ((color.G() << 3) & GREEN_MASK) | (color.B() >> 3));		
    return Color(color.R(), color.G(), color.B());
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE Orbiter_PocketFrog::GetRedColor(Pixel pixel)
{
	return (pixel & RED_MASK)   >> (RED_SHIFT - (8 - RED_BITS));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE Orbiter_PocketFrog::GetGreenColor(Pixel pixel)
{
	return (pixel & GREEN_MASK) >> (GREEN_SHIFT - (8 - GREEN_BITS));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ BYTE Orbiter_PocketFrog::GetBlueColor(Pixel pixel)
{
	return (pixel & BLUE_MASK)  << (8 - BLUE_BITS); 
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity)
{
    if(width <= 0 || height <= 0)
        return;

    //clipping
    if(x + width >= m_iImageWidth)
        width = m_iImageWidth - x - 1;

    if(y + height >= m_iImageHeight)
        height = m_iImageHeight - y - 1;
    
    CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->FillRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HollowRectangle(int x, int y, int width, int height, PlutoColor color)
{
    if(width <= 0 || height <= 0)
        return;

    //clipping
    if(x + width >= m_iImageWidth)
        width = m_iImageWidth - x - 1;

    if(y + height >= m_iImageHeight)
        height = m_iImageHeight - y - 1;

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->DrawRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ReplaceColorInRectangle(int x, int y, int width, int height, 
	PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
    if(width <= 0 || height <= 0 || width >= m_iImageWidth || height >= m_iImageHeight)
        return;

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Pixel pixelSrc = GetColor16(ColorToReplace);
	Pixel pixelDest = GetColor16(ReplacementColor);
	Pixel pixelCurrent;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            if(i + x >= 0 && j + y >= 0 && i + x < m_iImageWidth && j + y < m_iImageHeight)
            {
			    pixelCurrent = GetDisplay()->GetPixel(i + x, j + y);

    		    if(pixelSrc == pixelCurrent)
				    GetDisplay()->SetPixel(i + x, j + y, pixelDest);
            }
        }
    }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderText(string &TextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point)
{
    CHECK_STATUS();
    PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

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
                    pPieceTextStyle = m_mapTextStyle_Find(nTextStyle);

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

/*virtual*/ void Orbiter_PocketFrog::RenderText(HDC hdc,string &TextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectCalcLocation)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

    RECT rectLocation;
	rectLocation.left   = 0;
	rectLocation.top    = 0;

	HDC hdc_drawing=0;

	if( m_iRotation==0 )
	{
		hdc_drawing=hdc;
		rectLocation.right  = rPosition.Width;
		rectLocation.bottom = rPosition.Height;
	}
	else
	{
		if( m_iRotation==180 )
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

	if( m_iRotation==0 )
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
	CalcTextRectangle(rectLocation,rPosition,m_iRotation,iRealHeight,iPK_VertAlignment);

	if( m_iRotation==0 )
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
		for(int x=0;x<(m_iRotation==180 ? rPosition.Width : rPosition.Height);x++)
		{
			for(int y=0;y<(m_iRotation==180 ? rPosition.Height: rPosition.Width);y++)
			{
				COLORREF c = ::GetPixel(hdc_drawing,x,y);
				if( c==132 || c==128 )
					continue;  // Unchanged
				if( m_iRotation==90 )
					::SetPixel(hdc,rPosition.Right()-y,rPosition.Y+x,c);
				else if( m_iRotation==180 )
					::SetPixel(hdc,rPosition.Right()-x,rPosition.Bottom()-y,c);
				else if( m_iRotation==270 )
					::SetPixel(hdc,rPosition.X+y,rPosition.Bottom()-x,c);
			}
		}
		DeleteDC(hdc_drawing);
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Rect srcRect;
	srcRect.Set(point.X + pObj->m_rPosition.Left(), point.Y + pObj->m_rPosition.Top(), 
		point.X + pObj->m_rPosition.Right() + 1, point.Y + pObj->m_rPosition.Bottom() + 1);

    if(srcRect.right >= m_iImageWidth)
        srcRect.right = m_iImageWidth - 1;

    if(srcRect.bottom >= m_iImageHeight)
        srcRect.bottom = m_iImageHeight - 1;
                
	Surface *pSurface = GetDisplay()->CreateSurface(pObj->m_rPosition.Width, pObj->m_rPosition.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
    delete pRasterizer;

	if(pObj->m_pGraphicToUndoSelect)
		pObj->m_pGraphicToUndoSelect->Clear();

	pObj->m_pGraphicToUndoSelect = new PocketFrogGraphic(pSurface);
}

PlutoGraphic *Orbiter_PocketFrog::GetBackground( PlutoRectangle &rect )
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	if(rect.Width <= 0)
		rect.Width = 1;
	
	if(rect.Height <= 0)
		rect.Height = 1;

	//clipping
	if(rect.X + rect.Width >= m_iImageWidth && rect.Width > 0)
		rect.Width = m_iImageWidth - rect.X - 1;

	if(rect.Y + rect.Height >= m_iImageHeight && rect.Height > 0)
		rect.Height = m_iImageHeight - rect.Y - 1;

	Rect srcRect;
	srcRect.Set(rect.X, rect.Y, rect.Right() + 1, rect.Bottom() + 1);

	Surface *pSurface = GetDisplay()->CreateSurface(rect.Width, rect.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
    delete pRasterizer;
	
	return new PocketFrogGraphic(pSurface);
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *Orbiter_PocketFrog::CreateGraphic()
{
	return new PocketFrogGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderScreen()
{
	CHECK_STATUS();
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str() : " NO SCREEN"));
#endif
	
	if (m_bUseOpenGL)
	{
#ifndef WINCE
		Rect srcRect;
		srcRect.Set(0, 0, m_iImageWidth, m_iImageHeight);
		auto_ptr<Surface> spSurface(GetDisplay()->CreateSurface(m_iImageWidth, m_iImageHeight));
		auto_ptr<Rasterizer> spRasterizer(GetDisplay()->CreateRasterizer(spSurface.get()));
		spRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);
		m_spBeforeGraphic.reset(new PocketFrogGraphic(spSurface.get()));
		spRasterizer.release();
		spSurface.release();
#endif
	}

	if (m_pScreenHistory_Current)
	{
		PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		GetDisplay()->FillRect(0, 0, m_iImageWidth, m_iImageHeight, 0x0000);
	}

	m_bPoolRendering=true;
	Orbiter::RenderScreen();
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

	if (m_bUseOpenGL)
	{
#ifndef WINCE
		if(m_spAfterGraphic.get())
			m_spAfterGraphic->Initialize();

		m_spAfterGraphic.reset(new PocketFrogGraphic(GetDisplay()->GetBackBuffer()));

		if(m_pObj_SelectedLastScreen)
		{
			m_rectLastSelected = PlutoRectangle(m_pObj_SelectedLastScreen->m_rPosition);
			m_rectLastSelected.Y = m_iImageHeight - m_pObj_SelectedLastScreen->m_rPosition.Y;
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "No object selected? :o");
			m_rectLastSelected.X = 0;
			m_rectLastSelected.Y = m_iImageHeight;
			m_rectLastSelected.Width = 80;
			m_rectLastSelected.Height = 80;
		}

		m_pObj_SelectedLastScreen = NULL;

		StartAnimation();
#endif
	}
	else
	{
		TryToUpdate();
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RedrawObjects()
{
    Orbiter::RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
    bool bDisableAspectRatio, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
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
/*virtual*/ void Orbiter_PocketFrog::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::EndPaint()
{
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_WARNING, "End paint...");
#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::UpdateRect(PlutoRectangle rect, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

    PlutoRectangle localrect = rect;
    localrect.X += point.X;
    localrect.Y += point.Y;

	if(localrect.Width <= 0 || localrect.Height <= 0)
		return;

	//clipping the rectangle 
	if(localrect.X < 0)
		localrect.X = 0;

	if(localrect.Y < 0)
		localrect.Y = 0;

	if(localrect.Right() >= m_Width)
		localrect.Width = m_Width - localrect.X - 1;

	if(localrect.Bottom() >= m_Height)
		localrect.Height = m_Height - localrect.Y - 1;

	Rect rectUpdate;
	rectUpdate.Set(localrect.Left(), localrect.Top(), localrect.Right(), localrect.Bottom());
	GetDisplay()->Update(&rectUpdate);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::OnQuit()
{
	m_bQuit = true;
	pthread_cond_broadcast( &m_listMessageQueueCond );
	::PostMessage( m_hWnd, WM_CLOSE, 0, 0 );
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
	CHECK_STATUS();
	Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_PocketFrog::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog: need to cleanup orbiter...");

	if(NULL != m_pInstance)
	{
		if(::IsWindow(m_pInstance->m_hWnd))
			::DestroyWindow(m_pInstance->m_hWnd);
		
		m_pInstance->m_bQuit = true;	

		Orbiter_PocketFrog *pCopy = m_pInstance;
		m_pInstance = NULL;

		delete pCopy;
		pCopy = NULL;

		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog: orbiter deleted.");
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_PocketFrog::BuildOrbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen, bool bUseOpenGL)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog constructor.");
		m_pInstance = new Orbiter_PocketFrog(DeviceID, PK_DeviceTemplate, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen, bUseOpenGL);
		g_pCommand_Impl = m_pInstance;  
	}
	else
	{
		throw "Orbiter_PocketFrog already created!";
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ Orbiter_PocketFrog *Orbiter_PocketFrog::GetInstance()
{
	if(!m_pInstance)
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog:GetInstance The instance to orbiter is NULL");

	return m_pInstance;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::WriteStatusOutput(const char* pMessage)
{
	//
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
{
    /*
	CHECK_STATUS();

	HDC gdc;
	int iESC=SETPOWERMANAGEMENT;

	gdc = ::GetDC(NULL);
	if (ExtEscape(gdc, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&iESC, 0, NULL)!=0)		
	{
		VIDEO_POWER_MANAGEMENT vpm;
		vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
		vpm.DPMSVersion = 0x0001;
		vpm.PowerState = VideoPowerOn;
		ExtEscape(gdc, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL);
	}
	::ReleaseDC(NULL, gdc);
    */
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
    /*
	CHECK_STATUS();

	HDC gdc;
	int iESC=SETPOWERMANAGEMENT;

	gdc = ::GetDC(NULL);
	if (ExtEscape(gdc, QUERYESCSUPPORT, sizeof(int), (LPCSTR)&iESC, 0, NULL)!=0)		
	{
		VIDEO_POWER_MANAGEMENT vpm;
		vpm.Length = sizeof(VIDEO_POWER_MANAGEMENT);
		vpm.DPMSVersion = 0x0001;
		vpm.PowerState = VideoPowerOff;
		ExtEscape(gdc, SETPOWERMANAGEMENT, vpm.Length, (LPCSTR) &vpm, 0, NULL);
	}
	::ReleaseDC(NULL, gdc);
    */
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::TryToUpdate()
{
	CHECK_STATUS();

	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->Update();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ShowProgress(int nPercent)
{
	CHECK_STATUS();

	static int nCount = 0;
	if(++nCount % PROGRESS_UPDATE_STEP)
		return;

	int nProgressWidth = m_iImageWidth < PROGRESS_OPTIMAL_SIZE ? m_iImageWidth - PROGRESS_MAX : PROGRESS_OPTIMAL_SIZE;
	PlutoRectangle rect(
		(m_iImageWidth - nProgressWidth) / 2, m_iImageHeight - PROGRESS_BOTTOM_DISTANCE, 
		nProgressWidth, PROGRESS_HEIGHT
	);

	SolidRectangle(rect.Left(), rect.Top(), rect.Width, rect.Height, BorderColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width - 3, rect.Height - 3, FillBackgroundColor);
	SolidRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width * nPercent / PROGRESS_MAX - 3, rect.Height - 3, 
		PlutoColor(
			ProgressStartColor.R() + nPercent * (ProgressEndColor.R() - ProgressStartColor.R()) / PROGRESS_MAX, 
			ProgressStartColor.G() + nPercent * (ProgressEndColor.G() - ProgressStartColor.G()) / PROGRESS_MAX, 
			ProgressStartColor.B() + nPercent * (ProgressEndColor.B() - ProgressStartColor.B()) / PROGRESS_MAX
		)
	);

	DesignObjText text;
	string sText = StringUtils::ltos(nPercent) + "%";

	//avoid pool rendering
	m_bPoolRendering = false; 

	m_spTextStyle->m_ForeColor = TextShadowColor;
	text.m_rPosition = PlutoRectangle(rect.Left() + 1, rect.Top() + 1, rect.Width, rect.Height);
	RenderText(sText, &text, m_spTextStyle.get());

	m_spTextStyle->m_ForeColor = TextColor;
	text.m_rPosition = rect;
	RenderText(sText, &text, m_spTextStyle.get());

	//restore default
	m_bPoolRendering = true;

	Rect rectPocketFrog;
	rectPocketFrog.Set(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
    GetDisplay()->Update(&rectPocketFrog);
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_PocketFrog::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);
	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::GameSuspend()
{
	Game::GameSuspend();
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::GameResume()
{
	Game::GameResume();
}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_PocketFrog::IsFullScreen() 
{ 
	return m_bFullScreen; 
}
//-----------------------------------------------------------------------------------------------------
int Orbiter_PocketFrog::GetWindowWidth() { return m_iImageWidth;  } 
//-----------------------------------------------------------------------------------------------------
int Orbiter_PocketFrog::GetWindowHeight() { return m_iImageHeight; } 
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::PingFailed()
{
	g_pPlutoLogger->Write(LV_CRITICAL, "Ping Failed! About to reload .. ");

	ShowMainDialog();

	m_bQuit = true;
	m_bReload = true;
	OnQuit();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::AdjustWindowSize(int iOrbiterWidth, int iOrbiterHeight)
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
void Orbiter_PocketFrog::CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment)
{
	if( m_iRotation==0 ) 
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
	else if( m_iRotation==180 )
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
	else if( m_iRotation==90 || m_iRotation==270 )
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
/*virtual*/ bool Orbiter_PocketFrog::OnReplaceHandler(string sIP)
{
    if(sIP != m_sIPAddress)
    {
        string sMessage = "An Orbiter with the same device id was started on " + sIP + ".\r\nThis Orbiter will be closed.";
        PromptUser(sMessage);
        m_bQuit = true;
        exit(1);
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::OnUnexpectedDisconnect()
{
    //PromptUser("Orbiter have been disconnected. Orbiter will reload.");

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

    sCmdLine += "-d " + StringUtils::ltos(m_dwPK_Device);
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

    m_bQuit = true;
    exit(1); //die!!!
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int Orbiter_PocketFrog::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
    return PromptUserEx(sPrompt, p_mapPrompts);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::DisplayProgress(string sMessage, int nProgress)
{
    return DialogProgressEx(sMessage, nProgress);
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::DumpLocks() 
{
    string sMessage = 
        FormatMutexMessage(m_ScreenMutex) +
        FormatMutexMessage(m_VariableMutex) +
        FormatMutexMessage(m_DatagridMutex) +
        FormatMutexMessage(m_MaintThreadMutex) +
        FormatMutexMessage(m_NeedRedrawVarMutex);

#ifdef WINCE
    wchar_t MessageW[4096];
    mbstowcs(MessageW, sMessage.c_str(), 4096);
    ::MessageBox(m_hWnd, MessageW, TEXT("Orbiter mutexes"), MB_OK);
#else
    ::MessageBox(m_hWnd, sMessage.c_str(), TEXT("Orbiter mutexes"), MB_OK);
#endif
}
//-----------------------------------------------------------------------------------------------------
string Orbiter_PocketFrog::FormatMutexMessage(pluto_pthread_mutex_t& PlutoMutex)
{
    string sMessage =
        "Mutex '" + PlutoMutex.m_sName + "':\r\n" + 
        "    - " + StringUtils::ltos(PlutoMutex.m_NumLocks) + " lock(s)" + "\r\n" + 
        "    - " + PlutoMutex.m_sFileName + ":" + StringUtils::ltos(PlutoMutex.m_Line) + "\r\n";

    return sMessage;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::OpenGL_RenderFrame(void *data) //callback
{
#ifndef WINCE
	if(!m_spGLDesktop.get())
	{
		m_spGLDesktop.reset(new OrbiterGL3D(this));
		m_spGLDesktop->BeginAnimation();

		m_spGLDesktop->EffectBuilder->Widgets->ConfigureNextScreen(this->m_spAfterGraphic.get());
		m_spGLDesktop->EffectBuilder->Widgets->ConfigureOldScreen(this->m_spBeforeGraphic.get());
		GL2DBezierEffectTransit* Transit = (GL2DBezierEffectTransit*) m_spGLDesktop->EffectBuilder->
			CreateEffect(GL2D_EFFECT_BEZIER_TRANSIT, 1000);
		Transit->Configure(&m_rectLastSelected);
	}

	if (!m_spGLDesktop->EffectBuilder->HasEffects()) 
	{
		if (!m_bPaintDesktop) 
		{
			m_bPaintDesktop = true;
			m_spGLDesktop->Paint();
			m_spGLDesktop->EndAnimation();
			m_spGLDesktop.reset();
		}
	}
	else 
	{
		m_bPaintDesktop = false;
		m_spGLDesktop->Paint();

		CallMaintenanceInMiliseconds(0, (OrbiterCallBack) &Orbiter_PocketFrog::OpenGL_RenderFrame, 
			NULL, pe_NO);
	}
#endif
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_PocketFrog::StartAnimation()
{
	m_bPaintDesktop = false;
	CallMaintenanceInMiliseconds(0, (OrbiterCallBack) &Orbiter_PocketFrog::OpenGL_RenderFrame, 
		NULL, pe_NO);
}
//-----------------------------------------------------------------------------------------------------
