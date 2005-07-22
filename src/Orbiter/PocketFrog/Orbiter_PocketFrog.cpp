#include "Orbiter_PocketFrog.h"

#include "Wingdi.h"
#include "MainDialog.h"
#include "Resource.h"
#include "SelfUpdate.h"
#include "VirtualKeysTranslator.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 

using namespace Frog;
using namespace Frog::Internal;

#include "src/internal/graphicbuffer.h" 
#include <src/rasterizer.h>
//-----------------------------------------------------------------------------------------------------
const int ciCharWidth = 9;
const int ciCharHeight = 16;
const int ciSpaceHeight = 5;
//-----------------------------------------------------------------------------------------------------
#define RED_MASK_16		(0x1F << 11)
#define GREEN_MASK_16	(0x3F << 5)
#define BLUE_MASK_16	0x1F

#define MAX_STRING_LEN 1024
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
//-----------------------------------------------------------------------------------------------------
Orbiter_PocketFrog::Orbiter_PocketFrog(int DeviceID, string ServerAddress, string sLocalDirectory, 
									   bool bLocalMode, int nImageWidth, int nImageHeight, 
									   bool bFullScreen) :
		Orbiter(DeviceID, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth, nImageHeight)
{
    m_config.orientation      = ORIENTATION_WEST;
    m_config.splashScreenTime = 0;	
	m_bUpdating = false;
	m_bFullScreen=bFullScreen;
	m_bConnectionLost = false;
	m_bReload = false;
	m_bQuit = false;
	m_bWeCanRepeat = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Orbiter_PocketFrog::~Orbiter_PocketFrog()
{
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::GameInit()
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	Rect r;
	::GetClientRect( m_hWnd, &r );
	Surface* pLogoSurface = NULL;

#ifdef WINCE
	pLogoSurface = LoadImage( GetDisplay(), TEXT("\\Storage Card\\logo.gif") );
#else
	pLogoSurface = LoadImage( GetDisplay(), TEXT("logo.gif")); 
#endif

	if(pLogoSurface && pLogoSurface->m_height == m_iImageHeight && pLogoSurface->m_width == m_iImageWidth)
	{
		GetDisplay()->Blit( 0, 0, pLogoSurface );
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
        ShowMainDialog();
        MessageBox(TEXT("Orbiter cannot start because no screens are generated. \r\nTo generate screens, please use OrbiterGen."),
            TEXT("Pluto Orbiter"), MB_ICONHAND);

        exit(1);
    }

	if (!m_bLocalMode)
		CreateChildren();

	//loading font resources for wince
#ifdef WINCE
	HRSRC hResInfo	  = NULL;
	HGLOBAL hResource = NULL;
	hResInfo = FindResource(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_VGAROM), TEXT("FONTS"));
	hResource = LoadResource(_Module.GetModuleInstance(), hResInfo);
	VGAROMFont = (unsigned char*)LockResource(hResource);
#endif

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
    else if( wParam == VK_P && m_bControlDown)
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

    Orbiter::ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
/*static inline*/ Pixel Orbiter_PocketFrog::GetColor16(PlutoColor color)
{
	return (Pixel)(((color.R() << 8) & RED_MASK_16) | ((color.G() << 3) & GREEN_MASK_16) | (color.B() >> 3));		
}
//-----------------------------------------------------------------------------------------------------
/*static inline*/ BYTE Orbiter_PocketFrog::GetRedColor(Pixel pixel)
{
	return (pixel & RED_MASK >> RED_SHIFT);
}
//-----------------------------------------------------------------------------------------------------
/*static inline*/ BYTE Orbiter_PocketFrog::GetGreenColor(Pixel pixel)
{
	return (pixel & GREEN_MASK >> GREEN_SHIFT);
}
//-----------------------------------------------------------------------------------------------------
/*static inline*/ BYTE Orbiter_PocketFrog::GetBlueColor(Pixel pixel)
{
	return (pixel & BLUE_MASK >> BLUE_SHIFT);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SolidRectangle(int x, int y, int width, int height, PlutoColor color, int Opacity)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->FillRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HollowRectangle(int x, int y, int width, int height, PlutoColor color)
{
    if(width <= 0 || height <= 0)
        return;

	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->DrawRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ReplaceColorInRectangle(int x, int y, int width, int height, 
	PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
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
/*virtual*/ void Orbiter_PocketFrog::RenderText(class DesignObjText *Text,class TextStyle *pTextStyle, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	string TextToDisplay = SubstituteVariables(Text->m_sText, NULL, 0, 0).c_str();

	/*
	//temp
#ifdef WINCE

#if ( defined( PROFILING ) )
    clock_t clkStart = clock(  );
#endif

	int iNumChars = Text->m_rPosition.Width / ciCharWidth;
	int i, iPos;

	vector<string> vectStringsInterm;
	while(string::npos != (iPos = TextToDisplay.find("\n")))
	{
		string FirstString = TextToDisplay.substr(0, iPos - 1);
		string SecondString = TextToDisplay.substr(iPos + 1, TextToDisplay.length() - 1);

		StringUtils::Replace(&FirstString, "\n", "");
		StringUtils::Replace(&FirstString, "\r", "");

		vectStringsInterm.push_back(FirstString);
		TextToDisplay = SecondString;
	}

	StringUtils::Replace(&TextToDisplay, "\n", "");
	StringUtils::Replace(&TextToDisplay, "\r", "");
	vectStringsInterm.push_back(TextToDisplay);

	vector<string> vectStrings;
	for(i = 0; i < vectStringsInterm.size(); i++)
	{
		vector<string> vectStringsTemp;
		StringUtils::BreakIntoLines( vectStringsInterm[i], &vectStringsTemp, iNumChars );

		for(int j = 0; j < vectStringsTemp.size(); j++)
			vectStrings.push_back(vectStringsTemp[j]);
	}
	vectStringsInterm.clear();

	int Y = Text->m_rPosition.Y;
	int iTextRectHeight = vectStrings.size() * ciCharHeight + (vectStrings.size() - 1) * ciSpaceHeight;

	switch(Text->m_iPK_VertAlignment)
	{
		case VERTALIGNMENT_Top_CONST:
			break; 
		case VERTALIGNMENT_Middle_CONST:
			Y += (Text->m_rPosition.Height - iTextRectHeight) / 2;
			break;
		case VERTALIGNMENT_Bottom_CONST:
			Y += Text->m_rPosition.Height - iTextRectHeight;
			break;
	}

	if(Y < Text->m_rPosition.Y)
		Y = Text->m_rPosition.Y;

	wchar_t TextW[4096];

	Pixel color = GetColor16(pTextStyle->m_ForeColor);

	//workaround: pocketfrog has a bug somewhere.. doesn't render black text (a mask issue?)
	if(color == Color(0, 0, 0)) //it's black
		color = Color(7, 7, 7); //almost black :)

	for(i = 0; i < vectStrings.size(); i++)
	{
		mbstowcs(TextW, vectStrings[i].c_str(), 4096);	

		int X = Text->m_rPosition.X;
		int iTextRectWidth = ciCharWidth * vectStrings[i].length();

		switch(Text->m_iPK_HorizAlignment)
		{
			case HORIZALIGNMENT_Left_CONST:
				break;
			case HORIZALIGNMENT_Right_CONST:
				X += Text->m_rPosition.Width - iTextRectWidth;
				break;
            case HORIZALIGNMENT_Center_CONST:
                X += (Text->m_rPosition.Width - iTextRectWidth) / 2;
                break;
            default:
                X += (Text->m_rPosition.Width - iTextRectWidth) / 2;
                break; //HORIZALIGNMENT_Center_CONST
		}

		if(Y + i * (ciCharHeight + ciSpaceHeight) + ciCharHeight >= Text->m_rPosition.Y + Text->m_rPosition.Height) 
			break;

		GetDisplay()->DrawVGAText(VGAROMFont, TextW, DVT_NONE, point.X + X, point.Y + Y + i * (ciCharHeight + ciSpaceHeight), color);
	}
	vectStrings.clear();

#if ( defined( PROFILING ) )
    clock_t clkFinished = clock(  );
    g_pPlutoLogger->Write( LV_CONTROLLER, "RenderText_PocketFrog: %s took %d ms",
	    TextToDisplay.c_str(), clkFinished-clkStart );
#endif


#else //winxp/2000
  */

	RECT rectLocation;
	rectLocation.left   = 0;
	rectLocation.top    = 0;

	HDC hdc_drawing=0, hdc = GetDisplay()->GetBackBuffer()->GetDC(false);
	if( m_iRotation==0 )
	{
		hdc_drawing=hdc;
		rectLocation.right  = Text->m_rPosition.Width;
		rectLocation.bottom = Text->m_rPosition.Height;
	}
	else
	{
		if( m_iRotation==180 )
		{
			rectLocation.right  = Text->m_rPosition.Width;
			rectLocation.bottom = Text->m_rPosition.Height;
		}
		else
		{
			rectLocation.right  = Text->m_rPosition.Height;
			rectLocation.bottom = Text->m_rPosition.Width;
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

	lf.lfHeight		= pTextStyle->m_iPixelHeight;
	if( m_iRotation==0 )
		lf.lfQuality	= DRAFT_QUALITY;
	else
		lf.lfQuality	= NONANTIALIASED_QUALITY;
	lf.lfWeight		= pTextStyle->m_bBold ? FW_EXTRABOLD : FW_NORMAL;
	lf.lfItalic		= pTextStyle->m_bItalic;
	lf.lfUnderline	= pTextStyle->m_bUnderline;
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
    mbstowcs(wFaceName, pTextStyle->m_sFont.c_str(), 1024);	
	wcscpy(lf.lfFaceName, wFaceName);
#else
	strcpy(lf.lfFaceName, pTextStyle->m_sFont.c_str());
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
	if( iRealHeight>Text->m_rPosition.Height )
		iRealHeight=Text->m_rPosition.Height; // Crop to the area

	// Figure out where to put this
	CalcTextRectangle(rectLocation,Text->m_rPosition,m_iRotation,iRealHeight,Text->m_iPK_VertAlignment);

	if( m_iRotation==0 )
	{
		rectLocation.left += point.X;
		rectLocation.right += point.X;
		rectLocation.top += point.Y;
		rectLocation.bottom += point.Y;
	}

 	::SetTextColor(hdc_drawing, RGB(255,255,255));//pTextStyle->m_ForeColor.R(), pTextStyle->m_ForeColor.G(), pTextStyle->m_ForeColor.B()));
	::SetBkMode(hdc_drawing, TRANSPARENT);

	switch (Text->m_iPK_HorizAlignment)
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
		for(int x=0;x<(m_iRotation==180 ? Text->m_rPosition.Width : Text->m_rPosition.Height);x++)
		{
			for(int y=0;y<(m_iRotation==180 ? Text->m_rPosition.Height: Text->m_rPosition.Width);y++)
			{
				COLORREF c = ::GetPixel(hdc_drawing,x,y);
				if( c==132 || c==128 )
					continue;  // Unchanged
				if( m_iRotation==90 )
					::SetPixel(hdc,Text->m_rPosition.Right()-y,Text->m_rPosition.Y+x,c);
				else if( m_iRotation==180 )
					::SetPixel(hdc,Text->m_rPosition.Right()-x,Text->m_rPosition.Bottom()-y,c);
				else if( m_iRotation==270 )
					::SetPixel(hdc,Text->m_rPosition.X+y,Text->m_rPosition.Bottom()-x,c);
			}
		}
		bool b2=DeleteDC(hdc_drawing);
	}

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);
//#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{
	CHECK_STATUS();
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Rect srcRect;
	srcRect.Set(point.X + pObj->m_rPosition.Left(), point.Y + pObj->m_rPosition.Top(), point.X + pObj->m_rPosition.Right(), point.Y + pObj->m_rPosition.Bottom());

	Surface *pSurface = GetDisplay()->CreateSurface(pObj->m_rPosition.Width, pObj->m_rPosition.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);

	if(pObj->m_pGraphicToUndoSelect)
		pObj->m_pGraphicToUndoSelect->Clear();

	pObj->m_pGraphicToUndoSelect = new PocketFrogGraphic(pSurface);
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
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() : " NO SCREEN"));
#endif
	
    if (m_pScreenHistory_Current)
    {
        PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
		GetDisplay()->FillRect(0, 0, m_iImageWidth, m_iImageHeight, 0x0000);
    }

    Orbiter::RenderScreen();
	TryToUpdate();
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
		return;

if( pPlutoGraphic->m_Filename=="" )
int k=2;

	PocketFrogGraphic *pPocketFrogGraphic = (PocketFrogGraphic *) pPlutoGraphic;
	Surface *pSurface = pPocketFrogGraphic->m_pSurface;

	if(::IsBadReadPtr(pSurface->m_buffer->GetPixels(), pSurface->GetWidth() * pSurface->GetHeight() * 2))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "The surface has a bad pointer for pixels array (Surface: %p, pixels: %p)",
			pSurface, pSurface->m_buffer->GetPixels());
		pPlutoGraphic->Clear(); //force reload ocg
		return;
	}

    rectTotal.X += point.X;
    rectTotal.Y += point.Y;

	if(pSurface->GetWidth() == 0 || pSurface->GetHeight() == 0)
		return;
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
/*static*/ void Orbiter_PocketFrog::BuildOrbiter(int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog constructor.");
		m_pInstance = new Orbiter_PocketFrog(DeviceID, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen);
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
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
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
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::TryToUpdate()
{
	CHECK_STATUS();

	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->Update();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ShowProgress()
{
	CHECK_STATUS();

	static int Counter = 0;
	static PlutoColor green(0, 200, 0);
	static PlutoColor white(255, 255, 255);

	Counter++;

	int iProgressWidth = m_iImageWidth < 300 ? m_iImageWidth - 50 : 300;
	int iStart = (m_iImageWidth - iProgressWidth) / 2;
	int iStep = iProgressWidth / 45;

	if(!(Counter % 50))
	{
		SolidRectangle(iStart, m_iImageHeight - 50, iProgressWidth, 3, white);
		SolidRectangle(iStart, m_iImageHeight - 50, Counter / iStep, 3, green);
		PlutoRectangle rect(iStart, m_iImageHeight - 50, iProgressWidth, 3);
		UpdateRect(rect, PlutoPoint(0, 0));
	}
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
