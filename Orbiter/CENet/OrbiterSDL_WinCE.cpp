#include "OrbiterSDL_WinCE.h"
#include "StringUtils.h"
#include "SDL_syswm.h"
#include "MainDialog.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

const MAX_STRING_LEN = 4096;
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK SDLWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OrbiterSDL_WinCE *pOrbiter = OrbiterSDL_WinCE::GetInstance();

	LRESULT Result = pOrbiter->OldSDLWindowProc(hWnd, uMsg, wParam, lParam);

    switch(uMsg)
	{
		case WM_PAINT:
			pOrbiter->RenderTextObjectsWinCE(hWnd);
			break;

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
	m_bQuitWinCE=false;
}
//-----------------------------------------------------------------------------------------------------
OrbiterSDL_WinCE::~OrbiterSDL_WinCE()
{
	ClearWinCETextObjectsList();
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
}
//-----------------------------------------------------------------------------------------------------
/*static*/ OrbiterSDL_WinCE *OrbiterSDL_WinCE::GetInstance()
{
	if(NULL != m_pInstance)
		return m_pInstance;
	else
	{
		throw "OrbiterSDL_WinCE not created yet!";
		return NULL;
	}
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSDL_WinCE::RenderScreen()
{
	ClearWinCETextObjectsList();

	OrbiterSDL::RenderScreen();
	
	RenderTextObjectsWinCE(hSDLWindow);
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

	m_listTextWinCEObject.push_back(new TextWinCEObject(TextToDisplay, rect, pTextStyle));
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::RenderTextObjectsWinCE(HWND hWnd)
{
	if(hWnd)
	{
		HDC hDC = ::GetDC(hWnd);

		list<TextWinCEObject *>::iterator it;
		for(it = m_listTextWinCEObject.begin(); it != m_listTextWinCEObject.end(); it++)
		{
			TextWinCEObject* pTextWinCEObject = *it;
			RenderTextWinCE(hDC, pTextWinCEObject);
		}

		::ReleaseDC(hWnd, hDC);
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::RenderTextWinCE(HDC hDC, TextWinCEObject *pTextWinCEObject)
{
    string TextToDisplay = pTextWinCEObject->m_sText;

	TextStyle *pTextStyle = pTextWinCEObject->m_pTextStyle;
/*
	int m_iPK_Style,m_iPK_StyleVariation,m_iPK_Style_Selected,m_iPK_Style_Highlighed,m_iPK_Style_Alt;
	int m_iVersion; // This is for the selected, alt, or highlighted versions
	string m_sFont;
//	PlutoColor m_ForeColor,
		m_BackColor,m_ShadowColor;
//	int m_iPixelHeight;
//	bool m_bBold, m_bItalic, m_bUnderline;
	int m_iShadowX, m_iShadowY;  // Pixels to offset a drop shadow
	int m_iBorderStyle;
	int m_iPK_HorizAlignment,m_iPK_VertAlignment;
	int m_iRotate;
	void *m_pTTF_Font;
*/

    LOGFONT lf;
    HFONT hFontNew, hFontOld;

	// Clear out the lf structure to use when creating the font.
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight		= pTextStyle->m_iPixelHeight;
    lf.lfQuality	= DRAFT_QUALITY;
	lf.lfWeight		= pTextStyle->m_bBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic		= pTextStyle->m_bItalic;
	lf.lfUnderline	= pTextStyle->m_bUnderline;

	::SetTextColor(hDC, RGB(pTextStyle->m_ForeColor.R(), pTextStyle->m_ForeColor.G(), pTextStyle->m_ForeColor.B()));
	::SetBkMode(hDC, TRANSPARENT);	

    hFontNew = ::CreateFontIndirect(&lf);
    hFontOld = (HFONT) ::SelectObject(hDC, hFontNew);

	TextToDisplay = StringUtils::Replace(TextToDisplay, "\n", "\n\r");

	wchar_t wTextBuffer[MAX_STRING_LEN];
	mbstowcs(wTextBuffer, TextToDisplay.c_str(), MAX_STRING_LEN);

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
	::DrawText(hDC, wTextBuffer, TextToDisplay.length(), &(pTextWinCEObject->m_rectLocation), 
		DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT); 

	int iRealHeight = pTextWinCEObject->m_rectLocation.bottom - 
		pTextWinCEObject->m_rectLocation.top;

	pTextWinCEObject->m_rectLocation.top    = rectOld.top + (iOldHeight - iRealHeight) / 2;
	pTextWinCEObject->m_rectLocation.bottom = rectOld.bottom;

	pTextWinCEObject->m_rectLocation.left   = rectOld.left;
	pTextWinCEObject->m_rectLocation.right  = rectOld.right;

	//real render
	::DrawText(hDC, wTextBuffer, TextToDisplay.length(), &(pTextWinCEObject->m_rectLocation), 
		DT_WORDBREAK | DT_CENTER | DT_NOPREFIX); 

    ::SelectObject(hDC, hFontOld);
    ::DeleteObject(hFontNew);

	//restore initial rect
	pTextWinCEObject->m_rectLocation.left	= rectOld.left;
	pTextWinCEObject->m_rectLocation.top	= rectOld.top;
	pTextWinCEObject->m_rectLocation.right  = rectOld.right;
	pTextWinCEObject->m_rectLocation.bottom = rectOld.bottom;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void OrbiterSDL_WinCE::ClearWinCETextObjectsList()
{
	list<TextWinCEObject *>::iterator it;
	for(it = m_listTextWinCEObject.begin(); it != m_listTextWinCEObject.end(); it++)
	{
		TextWinCEObject* pTextWinCEObject = *it;

		delete pTextWinCEObject;
		pTextWinCEObject = NULL;
	}

	m_listTextWinCEObject.clear();
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
			m_bQuitWinCE = true;

			atexit(SDL_Quit);
			::PostMessage(hSDLWindow, WM_QUIT, 0L, 0L);

			SDL_Event event;
			event.type = SDL_USEREVENT;
			SDL_PushEvent(&event); 
		}
		else if( wParam == VK_F1 && m_bControlDown && m_bAltDown)
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