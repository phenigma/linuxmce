#include "Orbiter_PocketFrog.h"
#include "MainDialog.h"
#include "Resource.h"

#include "../pluto_main/Define_Button.h"
#include "../pluto_main/Define_Direction.h" 

#include "../pluto_main/Define_VertAlignment.h" 
#include "../pluto_main/Define_HorizAlignment.h" 

using namespace Frog;
using namespace Frog::Internal;

#include <src/rasterizer.h>
//-----------------------------------------------------------------------------------------------------
#define VK_A 0x41
#define VK_C 0x43
#define VK_E 0x45
#define VK_T 0x54
#define VK_P 0x50
#define VK_L 0x4C
#define VK_R 0x52
#define VK_Z 0x5A
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
extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
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
	g_pCommand_Impl = Orbiter_PocketFrog::GetInstance(); //it is possible that orbiter to be deleted and then 
													   //SocketCrashHandler to be called (so let's verify this)
	if( g_pCommand_Impl )
	{
		if( g_pPlutoLogger )
			g_pPlutoLogger->Write(LV_CRITICAL,"Socket problem.  Going to reload and quit");
		g_pCommand_Impl->OnReload();
	}
}
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

	m_bShiftDown = false;
	m_bControlDown = false;
	m_bAltDown = false;
	m_bRepeat = false;
	m_bCapsLock = false;
	m_bQuit = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Orbiter_PocketFrog::~Orbiter_PocketFrog()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_PocketFrog::GameInit()
{
	Surface* pLogoSurface = NULL;

#ifdef WINCE
	pLogoSurface = LoadImage( GetDisplay(), TEXT("\\Storage Card\\logo.gif") );
#else
	pLogoSurface = LoadImage( GetDisplay(), TEXT("logo.gif")); 
#endif

	if(pLogoSurface)
	{
		GetDisplay()->Blit( 0, 0, pLogoSurface );
		delete pLogoSurface;
	}
	else
	{
		GetDisplay()->FillRect(0, 0, m_iImageWidth, m_iImageHeight, 0x00);
	}
	GetDisplay()->Update();

	Initialize(gtPocketFrogGraphic);

	if(m_bQuit)
		return false;

	if(m_bReload)
		return false;

	if (!m_bLocalMode)
		CreateChildren();

#ifdef WINCE//todo: winx86
	HRSRC hResInfo	  = NULL;
	HGLOBAL hResource = NULL;
	hResInfo = FindResource(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_VGAROM), TEXT("FONTS"));
	hResource = LoadResource(_Module.GetModuleInstance(), hResInfo);
	VGAROMFont = (unsigned char*)LockResource(hResource);
#endif

#ifndef WINCE
	if(m_bFullScreen)
	{
		DEVMODE dmSettings;                          // Device Mode variable - Needed to change modes
		memset(&dmSettings,0,sizeof(dmSettings));    // Makes Sure Memory's Cleared

		// Get current settings -- This function fills our the settings
		// This makes sure NT and Win98 machines change correctly
		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
		{
			// Display error message if we couldn't get display settings
			return false;
		}

		dmSettings.dmPelsWidth = m_iImageWidth;                        // Set the desired Screen Width
		dmSettings.dmPelsHeight = m_iImageHeight;                      // Set the desired Screen Height
		dmSettings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;    // Set the flags saying we're changing the Screen Width and Height

		// This function actually changes the screen to full screen
		// CDS_FULLSCREEN Gets Rid Of Start Bar.
		// We always want to get a result from this function to check if we failed
		int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN); 
		// Check if we didn't recieved a good return message From the function
		if(result != DISP_CHANGE_SUCCESSFUL)
		{
			// Display the error message and quit the program
			PostQuitMessage(0);
			return false;
		}

		ModifyStyle(WS_TILEDWINDOW , 0);
		SetWindowPos(HWND_TOPMOST, 0, 0, m_iImageWidth, m_iImageHeight, SWP_SHOWWINDOW);
		SetForegroundWindow(m_hWnd);
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
	static WPARAM wOldParam = 0;
	static bool bLastEvent_KeyDown = false;

	if(uMsg == WM_KEYDOWN && bLastEvent_KeyDown && wOldParam == wParam) //this is a repeated key
		return;

	wOldParam = wParam;
	bLastEvent_KeyDown = uMsg == WM_KEYDOWN; //false if WM_KEYUP

    if (uMsg == WM_KEYDOWN)
    {
		if ( wParam == 81)
		{
			OnQuit();
			return;
		}

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
		RecordKeyboardAction(long(wParam));

        bool bHandled=false;
clock_t ccc=clock();
        m_bRepeat = m_cKeyDown && clock() - m_cKeyDown > CLOCKS_PER_SEC/2;
        m_cKeyDown=0;

        g_pPlutoLogger->Write(LV_STATUS, "key up %d  rept: %d  shif: %d",(int) wParam, (int) m_bRepeat, (int) m_bShiftDown);

#ifndef PHONEKEYS
        if(wParam >= VK_A && wParam <= VK_Z && !m_bControlDown && !m_bAltDown) // A-Z keys
        {
            if((!m_bCapsLock && !m_bShiftDown) || (m_bCapsLock && m_bShiftDown))
                bHandled = Orbiter::ButtonDown(BUTTON_a_CONST + int(wParam) - VK_A);
            else
                bHandled = Orbiter::ButtonDown(BUTTON_A_CONST + int(wParam) - VK_Z);
        }
#endif 
        if( wParam == VK_SHIFT)
            m_bShiftDown=false;
        else if( wParam == VK_CONTROL)
            m_bControlDown=false;
        else if( wParam == VK_MENU )
            m_bAltDown=false;
		else if( wParam == VK_F12) 
		{
			OnQuit();
		}
#ifdef WINCE
		else if( wParam == VK_F10) //A && m_bControlDown)
#else
		else if( wParam == VK_A && m_bControlDown)
#endif
		{
			ShowMainDialog();
		}
        else if( !m_bShiftDown && !m_bControlDown && !m_bAltDown && !m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=Orbiter::ButtonDown(BUTTON_0_CONST);
					break;
				case '1':
					bHandled=Orbiter::ButtonDown(BUTTON_1_CONST);
					break;
				case '2':
					bHandled=Orbiter::ButtonDown(BUTTON_2_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_3_CONST);
					break;
				case '4':
					bHandled=Orbiter::ButtonDown(BUTTON_4_CONST);
					break;
				case '5':
					bHandled=Orbiter::ButtonDown(BUTTON_5_CONST);
					break;
				case '6':
					bHandled=Orbiter::ButtonDown(BUTTON_6_CONST);
					break;
				case '7':
					bHandled=Orbiter::ButtonDown(BUTTON_7_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_8_CONST);
					break;
				case '9':
					bHandled=Orbiter::ButtonDown(BUTTON_9_CONST);
					break;

#ifdef PHONEKEYS
                case VK_C:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = Orbiter::ButtonDown(BUTTON_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = Orbiter::ButtonDown(BUTTON_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = Orbiter::ButtonDown(BUTTON_Pound_CONST);
                    break;
#endif 

				case VK_F1:
					bHandled=Orbiter::ButtonDown(BUTTON_F1_CONST);
					break;
				case VK_F2:
					bHandled=Orbiter::ButtonDown(BUTTON_F2_CONST);
					break;
				case VK_F3:
					bHandled=Orbiter::ButtonDown(BUTTON_F3_CONST);
					break;
				case VK_F4:
					bHandled=Orbiter::ButtonDown(BUTTON_F4_CONST);
					break;
				case VK_F5:
					bHandled=Orbiter::ButtonDown(BUTTON_F5_CONST);
					break;
				case VK_UP:
					bHandled=Orbiter::ButtonDown(BUTTON_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=Orbiter::ButtonDown(BUTTON_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=Orbiter::ButtonDown(BUTTON_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=Orbiter::ButtonDown(BUTTON_Right_Arrow_CONST);
					break;
				case VK_RETURN:
					bHandled=Orbiter::ButtonDown(BUTTON_Enter_CONST);
					break;
				case VK_BACK:
					bHandled=Orbiter::ButtonDown(BUTTON_Back_CONST);
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
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Up_Arrow_CONST);
					break;
				case VK_DOWN:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Down_Arrow_CONST);
					break;
				case VK_LEFT:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Left_Arrow_CONST);
					break;
				case VK_RIGHT:
					bHandled=Orbiter::ButtonDown(BUTTON_Shift_Right_Arrow_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_Pound_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_Asterisk_CONST);
					break;
            };
        }
		else if( m_bShiftDown && m_bControlDown && wParam==VK_F1 )
			GotoScreen(m_sMainMenu);
        else if( m_bRepeat )
        {
            switch (wParam)
            {
				case '0':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_0_CONST);
					break;
				case '1':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_1_CONST);
					break;
				case '2':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_2_CONST);
					break;
				case '3':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_3_CONST);
					break;
				case '4':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_4_CONST);
					break;
				case '5':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_5_CONST);
					break;
				case '6':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_6_CONST);
					break;
				case '7':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_7_CONST);
					break;
				case '8':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_8_CONST);
					break;
				case '9':
					bHandled=Orbiter::ButtonDown(BUTTON_Rept_9_CONST);
					break;
#ifdef PHONEKEYS
                case VK_C:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_C_CONST);
                    break;
                case VK_P:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Pencil_CONST);
                    break;
                case VK_T:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Talk_CONST);
                    break;
                case VK_E:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_End_CONST);
                    break;
                case VK_L:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Soft_left_CONST);
                    break;
                case VK_R:
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Phone_Soft_right_CONST);
                    break;
                case '*':
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Asterisk_CONST);
                    break;
				case '#':
                    bHandled = Orbiter::ButtonDown(BUTTON_Rept_Pound_CONST);
                    break;
#endif 
			}
		}
	}
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
	GetDisplay()->FillRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::HollowRectangle(int x, int y, int width, int height, PlutoColor color)
{
	GetDisplay()->DrawRect(x, y, x + width, y + height, GetColor16(color));
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ReplaceColorInRectangle(int x, int y, int width, int height, 
	PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{
	Pixel pixelSrc = GetColor16(ColorToReplace);
	Pixel pixelDest = GetColor16(ReplacementColor);
	Pixel pixelCurrent;

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
			pixelCurrent = GetDisplay()->GetPixel(i + x, j + y);

			const int max_diff = 20;

			if(
				abs(GetRedColor(pixelSrc)	- GetRedColor(pixelCurrent))  < max_diff	&&
				abs(GetGreenColor(pixelSrc) - GetGreenColor(pixelCurrent)) < max_diff	&&
				abs(GetBlueColor(pixelSrc)	- GetBlueColor(pixelCurrent)) < max_diff
			)
			{
				GetDisplay()->SetPixel(i + x, j + y, pixelDest);
			}
        }
    }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderText(class DesignObjText *Text,class TextStyle *pTextStyle)
{
	string TextToDisplay = SubstituteVariables(Text->m_sText, NULL, 0, 0).c_str();
	TextToDisplay = StringUtils::Replace(TextToDisplay, "\n", "\n\r");

	//temp
#ifdef WINCE

#if ( defined( PROFILING ) )
    clock_t clkStart = clock(  );
#endif

	int iNumChars = Text->m_rPosition.Width / ciCharWidth;
	vector<string> vectStrings;
	
	StringUtils::BreakIntoLines( TextToDisplay, &vectStrings, iNumChars );

	int Y = Text->m_rPosition.Y;
	int iTextRectHeight = vectStrings.size() * ciCharHeight + (vectStrings.size() - 1) * ciSpaceHeight;

	switch(pTextStyle->m_iPK_VertAlignment)
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

	wchar_t TextW[4096];

	Pixel color = GetColor16(pTextStyle->m_ForeColor);

	//workaround: pocketfrog has a bug somewhere.. doesn't render black text (a mask issue?)
	if(color == Color(0, 0, 0)) //it's black
		color = Color(7, 7, 7); //almost black :)

	for(int i = 0; i < vectStrings.size(); i++)
	{
		mbstowcs(TextW, vectStrings[i].c_str(), 4096);	
		GetDisplay()->DrawVGAText(VGAROMFont, TextW, DVT_NONE, Text->m_rPosition.X, Y + i * (ciCharHeight + ciSpaceHeight), color);
	}
	vectStrings.clear();

#if ( defined( PROFILING ) )
    clock_t clkFinished = clock(  );
    g_pPlutoLogger->Write( LV_CONTROLLER, "RenderText_PocketFrog: %s took %d ms",
	    TextToDisplay.c_str(), clkFinished-clkStart );
#endif


#else //winxp/2000

	HDC hdc = GetDisplay()->GetBackBuffer()->GetDC(false);
	
	LOGFONT lf;
	HFONT hFontNew, hFontOld;

//	wchar_t wTextBuffer[MAX_STRING_LEN];
//	mbstowcs(wTextBuffer, TextToDisplay.c_str(), MAX_STRING_LEN);

	//SDL_Color color;
	//color.r = pTextStyle->m_ForeColor.R();
	//color.g = pTextStyle->m_ForeColor.G();
	//color.b = pTextStyle->m_ForeColor.B();

	// Clear out the lf structure to use when creating the font.
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight		= pTextStyle->m_iPixelHeight;
	lf.lfQuality	= DRAFT_QUALITY;
	lf.lfWeight		= pTextStyle->m_bBold ? FW_EXTRABOLD : FW_NORMAL;
	lf.lfItalic		= pTextStyle->m_bItalic;
	lf.lfUnderline	= pTextStyle->m_bUnderline;

	//wchar_t wFontName[MAX_STRING_LEN];
	//mbstowcs(wFontName, pTextStyle->m_sFont.c_str(), MAX_STRING_LEN);
	//lstrcpy(lf.lfFaceName, wFontName);   
	strcpy(lf.lfFaceName, pTextStyle->m_sFont.c_str());

	hFontNew = ::CreateFontIndirect(&lf);
	hFontOld = (HFONT) ::SelectObject(hdc, hFontNew);
	
	RECT rectLocation;
	rectLocation.left   = Text->m_rPosition.X;
	rectLocation.top    = Text->m_rPosition.Y;
	rectLocation.right  = Text->m_rPosition.X + Text->m_rPosition.Width;
	rectLocation.bottom = Text->m_rPosition.Y + Text->m_rPosition.Height;

	int iOldHeight = rectLocation.bottom - rectLocation.top;

	RECT rectOld = 
	{
		rectLocation.left,
		rectLocation.top,
		rectLocation.right,
		rectLocation.bottom
	};

	//calculate rect first
	::DrawText(hdc, TextToDisplay.c_str(), int(TextToDisplay.length()), &rectLocation, 
		DT_WORDBREAK | DT_NOPREFIX | DT_CALCRECT); 

	int iRealHeight = rectLocation.bottom - rectLocation.top;

	rectLocation.top    = rectOld.top + (iOldHeight - iRealHeight) / 2;
	rectLocation.bottom = rectOld.bottom;
	rectLocation.left   = rectOld.left;
	rectLocation.right  = rectOld.right;

	::SetTextColor(hdc, RGB(pTextStyle->m_ForeColor.R(), pTextStyle->m_ForeColor.G(), pTextStyle->m_ForeColor.B()));
	::SetBkMode(hdc, TRANSPARENT);

	switch (pTextStyle->m_iPK_HorizAlignment)
	{
		case HORIZALIGNMENT_Center_CONST: 
			::DrawText(hdc, TextToDisplay.c_str(), int(TextToDisplay.length()), &rectLocation, 
				DT_WORDBREAK | DT_CENTER | DT_NOPREFIX); 
		break;
		
		case HORIZALIGNMENT_Left_CONST: 
			::DrawText(hdc, TextToDisplay.c_str(), int(TextToDisplay.length()), &rectLocation, 
				DT_WORDBREAK | DT_NOPREFIX); 
		break;
	}

	::SelectObject(hdc, hFontOld);
	::DeleteObject(hFontNew);

	GetDisplay()->GetBackBuffer()->ReleaseDC(hdc);

#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj)
{
	Rect srcRect;
	srcRect.Set(pObj->m_rPosition.Left(), pObj->m_rPosition.Top(), pObj->m_rPosition.Right(), pObj->m_rPosition.Bottom());

	Surface *pSurface = GetDisplay()->CreateSurface(pObj->m_rPosition.Width, pObj->m_rPosition.Height);
	Rasterizer *pRasterizer = GetDisplay()->CreateRasterizer(pSurface);
	pRasterizer->Blit(0, 0, GetDisplay()->GetBackBuffer(), &srcRect);

	//pRasterizer->Clear(
	//delete pRasterizer;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *Orbiter_PocketFrog::CreateGraphic()
{
	return new PocketFrogGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::RenderScreen()
{
	g_pPlutoLogger->Write(LV_STATUS,"$$$ RENDER SCREEN $$$ %s",(m_pScreenHistory_Current ? m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str() : " NO SCREEN"));

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
/*virtual*/ void Orbiter_PocketFrog::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, bool bDisableAspectRatio)
{
	if(!pPlutoGraphic || pPlutoGraphic->GraphicType_get() != gtPocketFrogGraphic)
		return;//nothing to render or not an pocket frog graphic

	if(pPlutoGraphic->IsEmpty())
		return;

	PocketFrogGraphic *pPocketFrogGraphic = (PocketFrogGraphic *) pPlutoGraphic;
	Surface *pSurface = pPocketFrogGraphic->m_pSurface;

	if(pSurface->GetWidth() == 0 || pSurface->GetHeight() == 0)
		return;
	else
		if(pSurface->GetWidth() != rectTotal.Width || pSurface->GetHeight() != rectTotal.Height)
		{
			Rect dest;	
			dest.Set(rectTotal.Left(), rectTotal.Top(), rectTotal.Right(), rectTotal.Bottom());
			GetDisplay()->BlitStretch(dest, pSurface);

			g_pPlutoLogger->Write(LV_STATUS, "Need to stretch picture: %d, %d, %d, %d", 
				rectTotal.Left(), rectTotal.Top(), rectTotal.Right(), rectTotal.Bottom());
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
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::UpdateRect(PlutoRectangle rect)
{
	while(m_bUpdating)
		Sleep(30);

	if(!m_bUpdating)
	{
		m_bUpdating = true;
		Rect rectUpdate;
		rectUpdate.Set(rect.Left(), rect.Top(), rect.Right(), rect.Bottom());
		GetDisplay()->Update(&rectUpdate);
		m_bUpdating = false;
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::OnQuit()
{
	Shutdown(); //just in case
	PostMessage( WM_CLOSE, 0, 0 ); //break pocketfrog loop
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::Initialize(GraphicType Type, int iPK_Room, int iPK_EntertainArea)
{
	Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_PocketFrog::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "Orbiter_PocketFrog: need to cleanup orbiter...");
	if(NULL != m_pInstance)
	{
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
	if(m_bQuit)
		return;

	while(m_bUpdating)
		Sleep(30);

	if(!m_bUpdating)
	{
		m_bUpdating = true;
		GetDisplay()->Update();
		m_bUpdating = false;
	}
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_PocketFrog::ShowProgress()
{
	static int Counter = 0;
	static PlutoColor green(0, 200, 0);
	static PlutoColor white(255, 255, 255);

	Counter++;

	if(!(Counter % 50))
	{
		SolidRectangle(300, m_iImageHeight - 50, 200, 3, white);
		SolidRectangle(300, m_iImageHeight - 50, Counter / 9, 3, green);
		PlutoRectangle rect(300, m_iImageHeight - 50, 200, 3);
		UpdateRect(rect);
	}
}
//-----------------------------------------------------------------------------------------------------