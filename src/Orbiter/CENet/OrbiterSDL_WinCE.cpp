#include "OrbiterSDL_WinCE.h"
#include "StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "../pluto_main/Define_HorizAlignment.h"
#include "../pluto_main/Define_VertAlignment.h"

#include "SelfUpdate.h"

//#define PROFILING_CE

const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
//-----------------------------------------------------------------------------------------------------
void DeadlockHandler(PlutoLock *pPlutoLock)
{
	// This isn't graceful, but for the moment in the event of a deadlock we'll just kill everything and force a reload
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Deadlock problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
//-----------------------------------------------------------------------------------------------------
void SocketCrashHandler(Socket *pSocket)
{
	// This isn't graceful, but for the moment in the event of a socket crash we'll just kill everything and force a reload
	g_pCommand_Impl = OrbiterSDL_WinCE::GetInstance(); //it is possible that orbiter to be deleted and then 
													   //SocketCrashHandler to be called (so let's verify this)
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
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

    m_bShiftDown = false;
	m_bControlDown = false;
	m_bAltDown = false;
	m_bRepeat = false;
	m_bCapsLock = false;
    m_cKeyDown=0;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE::~OrbiterSDL_WinCE()
{
	//restore old sdl windowproc
	::SetWindowLong(hSDLWindow, GWL_WNDPROC, reinterpret_cast<long>(OldSDLWindowProc));	
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::BuildOrbiterSDL_WinCE(
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

	g_pDeadlockHandler=DeadlockHandler;
	g_pSocketCrashHandler=SocketCrashHandler;
	g_pCommand_Impl=m_pInstance;
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
	 SelectObject(m_hdc, fontInfo.bitmap);

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
    if (uMsg == WM_KEYDOWN)
    {
        g_pPlutoLogger->Write(LV_STATUS, "Key pressed event");
		switch (wParam)
        {
			case VK_SHIFT:
				m_bShiftDown=true;
				break;
			case VK_CONTROL:
				m_bControlDown=true;
				break;
			case VK_MENU:
				m_bAltDown=true;
				break;
			case VK_CAPITAL:
				m_bCapsLock = !m_bCapsLock;
				break;
			default:
				m_cKeyDown = clock();  // We don't care how long the shift, ctrl or alt are held down, but the other keys do matter
				break;
        }
    }
    else if (uMsg == WM_KEYUP)
    {
		RecordKeyboardAction(wParam);

        bool bHandled=false;
        m_bRepeat = m_cKeyDown && clock() - m_cKeyDown > CLOCKS_PER_SEC/2;
        m_cKeyDown=0;

        g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) wParam, (int) m_bRepeat, (int) m_bShiftDown);

        if( wParam == VK_SHIFT)
            m_bShiftDown=false;
        else if( wParam == VK_CONTROL)
            m_bControlDown=false;
        else if( wParam == VK_MENU )
            m_bAltDown=false;
		else if( wParam == VK_ESCAPE && m_bControlDown && m_bAltDown)
		{
			m_bQuit = true;

			atexit(SDL_Quit);
			::PostMessage(hSDLWindow, WM_QUIT, 0L, 0L);

			SDL_Event event;
			event.type = SDL_USEREVENT;
			SDL_PushEvent(&event); 
		}
		else if( wParam == VK_F10)
		{
			ShowMainDialog();
		}
        else if( !m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=ButtonDown(BUTTON_0_CONST);
					break;
				case '1':
					bHandled=ButtonDown(BUTTON_1_CONST);
					break;
				case '2':
					bHandled=ButtonDown(BUTTON_2_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_3_CONST);
					break;
				case '4':
					bHandled=ButtonDown(BUTTON_4_CONST);
					break;
				case '5':
					bHandled=ButtonDown(BUTTON_5_CONST);
					break;
				case '6':
					bHandled=ButtonDown(BUTTON_6_CONST);
					break;
				case '7':
					bHandled=ButtonDown(BUTTON_7_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_8_CONST);
					break;
				case '9':
					bHandled=ButtonDown(BUTTON_9_CONST);
					break;
				case VK_F1:
					bHandled=ButtonDown(BUTTON_F1_CONST);
					break;
				case VK_F2:
					bHandled=ButtonDown(BUTTON_F2_CONST);
					break;
				case VK_F3:
					bHandled=ButtonDown(BUTTON_F3_CONST);
					break;
				case VK_F4:
					bHandled=ButtonDown(BUTTON_F4_CONST);
					break;
				case VK_F5:
					bHandled=ButtonDown(BUTTON_F5_CONST);
					break;
				case VK_UP:
					bHandled=ButtonDown(BUTTON_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=ButtonDown(BUTTON_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=ButtonDown(BUTTON_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=ButtonDown(BUTTON_Right_Arrow_CONST);
					break;
				case VK_RETURN:
					bHandled=ButtonDown(BUTTON_Enter_CONST);
					break;
				case VK_BACK:
					bHandled=ButtonDown(BUTTON_Back_CONST);
					break;
				default:
					g_pPlutoLogger->Write(LV_STATUS, "Unknown key: %d", (int) wParam);
            };
        } 
        else if( m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case VK_UP:
					bHandled=ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_Pound_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_Asterisk_CONST);
					break;
            };
        }
        else if( m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=ButtonDown(BUTTON_Rept_0_CONST);
					break;
				case '1':
					bHandled=ButtonDown(BUTTON_Rept_1_CONST);
					break;
				case '2':
					bHandled=ButtonDown(BUTTON_Rept_2_CONST);
					break;
				case '3':
					bHandled=ButtonDown(BUTTON_Rept_3_CONST);
					break;
				case '4':
					bHandled=ButtonDown(BUTTON_Rept_4_CONST);
					break;
				case '5':
					bHandled=ButtonDown(BUTTON_Rept_5_CONST);
					break;
				case '6':
					bHandled=ButtonDown(BUTTON_Rept_6_CONST);
					break;
				case '7':
					bHandled=ButtonDown(BUTTON_Rept_7_CONST);
					break;
				case '8':
					bHandled=ButtonDown(BUTTON_Rept_8_CONST);
					break;
				case '9':
					bHandled=ButtonDown(BUTTON_Rept_9_CONST);
					break;
			}
		}
	}
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
	g_pPlutoLogger->Write(LV_STATUS, "OnQuit()");

	m_bQuit = true;
	m_bConnectionLost = true;

	//::ShowWindow(hSDLWindow, SW_HIDE);
	ShowMainDialog();

	SDL_Event event;
	event.type = SDL_USEREVENT;
	SDL_PushEvent(&event); 
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
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterSDL_WinCE::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
{

}
//-----------------------------------------------------------------------------------------------------
