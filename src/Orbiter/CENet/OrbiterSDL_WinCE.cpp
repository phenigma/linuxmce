#include <WINDOWS.H>

#include "OrbiterSDL_WinCE.h"
#include "StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"
#include "VirtualKeysTranslator.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "../pluto_main/Define_HorizAlignment.h"
#include "../pluto_main/Define_VertAlignment.h"

#include "SelfUpdate.h"

//#define PROFILING_CE

const MAX_STRING_LEN = 4096;
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
LRESULT CALLBACK SDLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OrbiterSDL_WinCE *pOrbiter = OrbiterSDL_WinCE::GetInstance();

	if(pOrbiter == NULL)
		return 0L;

	LRESULT Result = pOrbiter->OldSDLWindowProc(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
			pOrbiter->HandleKeyEvents(uMsg, wParam, lParam);
			break;
	}

	return Result;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE *OrbiterSDL_WinCE::m_pInstance = NULL; //the one and only
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE::OrbiterSDL_WinCE(int DeviceID, string ServerAddress, string sLocalDirectory, 
								   bool bLocalMode, int nImageWidth, int nImageHeight, 
								   bool bFullScreen) :
	OrbiterSDL(DeviceID, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen)
{
	//todo
	hSDLWindow = ::FindWindow(TEXT("SDL_app"), NULL);

	if(NULL != hSDLWindow)
	{
		OldSDLWindowProc = reinterpret_cast<WNDPROC>(::SetWindowLong(hSDLWindow, 
			GWL_WNDPROC, reinterpret_cast<long>(SDLWindowProc)));
	}
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE::~OrbiterSDL_WinCE()
{
	//restore old sdl windowproc
	::SetWindowLong(hSDLWindow, GWL_WNDPROC, reinterpret_cast<long>(OldSDLWindowProc));	
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::BuildOrbiter(
	int DeviceID, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
	int nImageWidth, int nImageHeight, bool bFullScreen
)
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new OrbiterSDL_WinCE(DeviceID, ServerAddress, 
				sLocalDirectory, bLocalMode, nImageWidth, 
				nImageHeight, bFullScreen);
	}
	else
	{
		throw "OrbiterSDL_WinCE already created!";
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::Cleanup()
{
	if(NULL != m_pInstance)
	{
		//restore windowproc
     	::SetWindowLong(m_pInstance->hSDLWindow, GWL_WNDPROC, reinterpret_cast<long>(m_pInstance->OldSDLWindowProc));

		delete m_pInstance;
		m_pInstance = NULL;
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ OrbiterSDL_WinCE *OrbiterSDL_WinCE::GetInstance()
{
	if(NULL != m_pInstance)
		return m_pInstance;
	else
	{
		return NULL;
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_WinCE::RenderText(DesignObjText *Text,TextStyle *pTextStyle)
{
	RECT rect;
	rect.left   = Text->m_rPosition.X;
	rect.top    = Text->m_rPosition.Y;
	rect.right  = Text->m_rPosition.X + Text->m_rPosition.Width;
	rect.bottom = Text->m_rPosition.Y + Text->m_rPosition.Height;

	string TextToDisplay = SubstituteVariables(Text->m_sText, NULL, 0, 0).c_str();

	TextWinCEObject *pTextWinCEObject = new TextWinCEObject(TextToDisplay, rect, pTextStyle);

#if ( defined( PROFILING_CE ) )
    clock_t clkStart = clock(  );
#endif
	
	RenderTextWinCE(pTextWinCEObject);

#if ( defined( PROFILING_CE ) )
    clock_t clkFinished = clock(  );
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "RenderTextCE: %s took %d ms",
            TextToDisplay.c_str(  ), clkFinished-clkStart );
    }
#endif

	delete pTextWinCEObject;
	pTextWinCEObject = NULL;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::RenderTextWinCE(TextWinCEObject *pTextWinCEObject)
{
#if ( defined( PROFILING_CE ) )
    clock_t clkStartx = clock(  );
#endif

     GDI_FontInfo                fontInfo;
     HDC                         hdc;
     HDC                         m_hdc;
     LPBITMAPINFO                pbmi;
     SIZE                        size;

     // Create a memory DC for rendering our text into
     hdc = GetDC(HWND_DESKTOP);
     m_hdc = CreateCompatibleDC(hdc);
     ReleaseDC(NULL, hdc);
     if (m_hdc == NULL) 
	 {
         return;
     }

    LOGFONT lf;
    HFONT hFontNew, hFontOld;

    string TextToDisplay = pTextWinCEObject->m_sText;
	TextToDisplay = StringUtils::Replace(TextToDisplay, "\n", "\n\r");

	TextStyle *pTextStyle = pTextWinCEObject->m_pTextStyle;

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, TextToDisplay.c_str(), MAX_STRING_LEN);

	SDL_Color color;
	color.r = pTextStyle->m_ForeColor.R();
	color.g = pTextStyle->m_ForeColor.G();
	color.b = pTextStyle->m_ForeColor.B();

	// Clear out the lf structure to use when creating the font.
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight		= pTextStyle->m_iPixelHeight;
    lf.lfQuality	= DRAFT_QUALITY;
	lf.lfWeight		= pTextStyle->m_bBold ? FW_MEDIUM : FW_NORMAL;
	lf.lfItalic		= pTextStyle->m_bItalic;
	lf.lfUnderline	= pTextStyle->m_bUnderline;

	wchar_t wFontName[MAX_STRING_LEN];
	mbstowcs(wFontName, pTextStyle->m_sFont.c_str(), MAX_STRING_LEN);
	lstrcpy(lf.lfFaceName, wFontName);   

    hFontNew = ::CreateFontIndirect(&lf);
    hFontOld = (HFONT) ::SelectObject(m_hdc, hFontNew);

	int iOldHeight = pTextWinCEObject->m_rectLocation.bottom - 
		pTextWinCEObject->m_rectLocation.top;

	RECT rectOld = 
	{
		pTextWinCEObject->m_rectLocation.left,
		pTextWinCEObject->m_rectLocation.top,
		pTextWinCEObject->m_rectLocation.right,
		pTextWinCEObject->m_rectLocation.bottom
	};

	//calculate rect first
	::DrawText(m_hdc, wTextBuffer, TextToDisplay.length(), &(pTextWinCEObject->m_rectLocation), 
		DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT); 

	int iRealHeight = pTextWinCEObject->m_rectLocation.bottom - 
		pTextWinCEObject->m_rectLocation.top;

	pTextWinCEObject->m_rectLocation.top    = rectOld.top + (iOldHeight - iRealHeight) / 2;
	pTextWinCEObject->m_rectLocation.bottom = rectOld.bottom;

	pTextWinCEObject->m_rectLocation.left   = rectOld.left;
	pTextWinCEObject->m_rectLocation.right  = rectOld.right;

	fontInfo.bmpWidth = rectOld.right - rectOld.left + 2;
    fontInfo.bmpHeight = rectOld.bottom - rectOld.top + 2 ;

	 // Create a dib section for containing the bits
	 pbmi = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFO) +
			 PALETTE_SIZE * sizeof(RGBQUAD));

	 if (pbmi == NULL)
	 {
		 DeleteDC(m_hdc);
		 return;
	 }

	 pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	 pbmi->bmiHeader.biWidth = fontInfo.bmpWidth;
	 // negative height = top-down
	 pbmi->bmiHeader.biHeight = -1 * fontInfo.bmpHeight;
	 pbmi->bmiHeader.biPlanes = 1;
	 pbmi->bmiHeader.biBitCount = 8; // 8bpp makes it easy to get data

	 pbmi->bmiHeader.biCompression = BI_RGB;
	 pbmi->bmiHeader.biXPelsPerMeter = 0;
	 pbmi->bmiHeader.biYPelsPerMeter = 0;
	 pbmi->bmiHeader.biClrUsed = PALETTE_SIZE;
	 pbmi->bmiHeader.biClrImportant = PALETTE_SIZE;

	 pbmi->bmiHeader.biSizeImage = WIDTHBYTES(fontInfo.bmpWidth * 8) *
			 fontInfo.bmpHeight;

	 // Just a plain monochrome palette
	 pbmi->bmiColors[0].rgbRed = 0;
	 pbmi->bmiColors[0].rgbGreen = 0;
	 pbmi->bmiColors[0].rgbBlue = 0;
	 pbmi->bmiColors[1].rgbRed = 255;
	 pbmi->bmiColors[1].rgbGreen = 255;
	 pbmi->bmiColors[1].rgbBlue = 255;

	 // Create a DIB section that we can use to read the font bits out of
	 fontInfo.bitmap = CreateDIBSection(m_hdc, pbmi,
				   DIB_RGB_COLORS, (void **) &fontInfo.bmpBits, NULL, 0);
	 LocalFree(pbmi);
	 if (fontInfo.bitmap == NULL)
	 {
		 DeleteDC(m_hdc);
		 return;
	 }

	 // Set up our memory DC with the font and bitmap
	::SelectObject(m_hdc, fontInfo.bitmap);

	::SetTextColor(m_hdc, RGB(color.r, color.g, color.b));
	::SetBkMode(m_hdc, TRANSPARENT);

	bool bFontColorIsBlack = color.r == 0 && color.g == 0 && color.b == 0;

	 // Output text to our memory DC (the bits end up in our DIB section)
	if(bFontColorIsBlack)
		PatBlt(m_hdc, 0, 0, fontInfo.bmpWidth, fontInfo.bmpHeight, WHITENESS);
	else
		PatBlt(m_hdc, 0, 0, fontInfo.bmpWidth, fontInfo.bmpHeight, BLACKNESS);

	 RECT rect = { 0, 0, fontInfo.bmpWidth, fontInfo.bmpHeight };

	//real render
	
	 switch (pTextStyle->m_iPK_HorizAlignment)
	 {
		case HORIZALIGNMENT_Center_CONST: 
				::DrawText(m_hdc, wTextBuffer, TextToDisplay.length(), &rect /*&(pTextWinCEObject->m_rectLocation)*/, 
					DT_WORDBREAK | DT_CENTER | DT_NOPREFIX); 

				break;
		case HORIZALIGNMENT_Left_CONST: 
				::DrawText(m_hdc, wTextBuffer, TextToDisplay.length(), &rect /*&(pTextWinCEObject->m_rectLocation)*/, 
					DT_WORDBREAK | DT_NOPREFIX); 

				break;
	 }
			
	::SelectObject(m_hdc, hFontOld);
	::DeleteObject(hFontNew);

#if ( defined( PROFILING_CE ) )
    clock_t clkFinishedx = clock(  );
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "in rendertextce ... gdi stuff: %s took %d ms",
            TextToDisplay.c_str(  ), clkFinishedx-clkStartx );
    }
#endif


#if ( defined( PROFILING_CE ) )
    clock_t clkStart = clock(  );
#endif
	
	 SDL_Surface * surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			   fontInfo.bmpWidth, fontInfo.bmpHeight, 32,
			   RMASK, GMASK, BMASK, AMASK);
	 
	 if (SDL_MUSTLOCK(surface)) {
		 SDL_LockSurface(surface);
	 }

	 Uint32 * destPixels = (Uint32 *) surface->pixels;

	 LPSTR lpsrc = fontInfo.bmpBits;
	 for (int ii = 0; ii < fontInfo.bmpHeight; ii++) {
		 for (int jj = 0; jj < fontInfo.bmpWidth; jj++) 
		 {
			if(!bFontColorIsBlack)
			{
				// If lpsrc[j] is 0, then it's a black pixel (opaque)
				// otherwise it's white (transparent)
				if (lpsrc[jj]) 
					destPixels[jj] = ::SDL_MapRGBA(surface->format, color.r, color.g, color.b, 0xff);
				else 
					destPixels[jj] = ::SDL_MapRGBA(surface->format, 0, 0, 0, 0);
			}
			else
			{
				// If lpsrc[j] is 1, then it's a white pixel (opaque)
				// otherwise it's black (transparent)
				if (!lpsrc[jj]) 
					destPixels[jj] = ::SDL_MapRGBA(surface->format, color.r, color.g, color.b, 0xff);
				else 
					destPixels[jj] = ::SDL_MapRGBA(surface->format, 0xff, 0xff, 0xff, 0);
			}
		 }
		 lpsrc += WIDTHBYTES(fontInfo.bmpWidth * 8);
		 destPixels += fontInfo.bmpWidth;
	 }

#if ( defined( PROFILING_CE ) )
    clock_t clkFinished = clock(  );
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "DC -> surface: %s took %d ms",
            TextToDisplay.c_str(  ), clkFinished-clkStart );
    }
#endif

	 if (SDL_MUSTLOCK(surface)) {
		 SDL_UnlockSurface(surface);
	}

#if ( defined( PROFILING_CE ) )
    clock_t clkStartxx = clock(  );
#endif

    DeleteDC(m_hdc);
    DeleteObject(fontInfo.bitmap);

    SDL_Rect TextLocation;
    TextLocation.x = pTextWinCEObject->m_rectLocation.left;
    TextLocation.y = pTextWinCEObject->m_rectLocation.top;
    TextLocation.w = pTextWinCEObject->m_rectLocation.right - pTextWinCEObject->m_rectLocation.left;
    TextLocation.h = pTextWinCEObject->m_rectLocation.bottom - pTextWinCEObject->m_rectLocation.top;

	SDL_BlitSurface(surface, NULL, m_pScreenImage, &TextLocation);
	SDL_FreeSurface(surface);

#if ( defined( PROFILING_CE ) )
    clock_t clkFinishedxx = clock(  );
    if(  m_pScreenHistory_Current   )
    {
        g_pPlutoLogger->Write( LV_CONTROLLER, "SDL stuff: %s took %d ms",
            TextToDisplay.c_str(  ), clkFinishedxx-clkStartxx );
    }
#endif

}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_WinCE::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Orbiter::Event orbiterEvent;
    orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;
    orbiterEvent.data.button.m_iPK_Button = 0;

    if(uMsg == WM_KEYDOWN)
        orbiterEvent.type = Orbiter::Event::BUTTON_DOWN;
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

    Orbiter::ProcessEvent(orbiterEvent);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_WinCE::WriteStatusOutput(const char* pMessage)
{
	RECT rect = { 0, 0, m_iImageWidth, m_iImageHeight };

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, pMessage, MAX_STRING_LEN);

	HDC hDC = ::GetDC(hSDLWindow);

	::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));

	::SetBkColor(hDC, RGB(0, 0, 0));
	::SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	::SetBkMode(hDC, OPAQUE);	

	::DrawText(hDC, wTextBuffer, string(pMessage).length(), &rect, 
		DT_NOPREFIX | DT_VCENTER | DT_CENTER | DT_SINGLELINE); 

	::ReleaseDC(hSDLWindow, hDC);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_WinCE::OnQuit()
{
	g_pPlutoLogger->Write(LV_STATUS, "OnQuit starting");

	m_bQuit = true;
	m_bConnectionLost = true;

	//::ShowWindow(hSDLWindow, SW_HIDE);
	g_pPlutoLogger->Write(LV_STATUS, "Minimizing orbiter");
	ShowMainDialog();

	g_pPlutoLogger->Write(LV_STATUS, "Sending dummy SDL_MOUSEMOTION");
	SDL_Event event;
	event.type = SDL_MOUSEMOTION;
	SDL_PushEvent(&event); 

	g_pPlutoLogger->Write(LV_STATUS, "OnQuit() exiting");
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSDL_WinCE::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);

	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL_WinCE::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
{
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
/*virtual*/ void OrbiterSDL_WinCE::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{
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
