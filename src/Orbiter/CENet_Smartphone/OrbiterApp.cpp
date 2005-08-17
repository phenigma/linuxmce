#include "OrbiterApp.h"

#include "DCE/Logger.h"
#include "BD/BDCommandProcessor_Smartphone_Bluetooth.h"
#include "BD/BD_WhatDoYouHave.h"
#include "VIPShared/BD_PC_KeyWasPressed.h"
#include "VIPShared/BD_PC_GetSignalStrength.h"
#include "pluto_main/Define_Button.h"


using namespace DCE;

#include <string>
using namespace std;

#define APP_WIDTH  176
#define APP_HEIGHT 220
//---------------------------------------------------------------------------------------------------------
CComModule _Module; 
//-----------------------------------------------------------------------------------------------------
#define CHECK_STATUS() { if(m_bQuit) return; }
//-----------------------------------------------------------------------------------------------------
OrbiterApp *OrbiterApp::m_pInstance = NULL; //the one and only
//---------------------------------------------------------------------------------------------------------
OrbiterApp::OrbiterApp() : m_ScreenMutex("rendering")
{
	m_bQuit = false;

	m_iImageWidth = APP_WIDTH;
	m_iImageHeight = APP_HEIGHT;

    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_ScreenMutex.Init( &m_MutexAttr );

	m_pBDCommandProcessor = new BDCommandProcessor_Smartphone_Bluetooth();
	m_pBDCommandProcessor->Start();

	m_pInstance = this;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterApp::~OrbiterApp()
{
	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_ScreenMutex.mutex);

	delete m_pBDCommandProcessor;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameSuspend()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameResume()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::IsFullScreen()
{
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterApp::GetWindowWidth()
{
	return m_iImageWidth;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ int OrbiterApp::GetWindowHeight()
{
	return m_iImageHeight;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::GameInit()
{
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);

	Rect r;
	::GetClientRect( m_hWnd, &r );
	Surface* pLogoSurface = NULL;

    string sLogoPath = g_sBinaryPath + "logo.gif";
    sLogoPath = StringUtils::Replace(sLogoPath, "/", "\\");

    wchar_t wPath[4096];
    mbstowcs(wPath, sLogoPath.c_str(), 4096);	

	pLogoSurface = LoadImage( GetDisplay(), wPath);

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
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameEnd()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::GameLoop()
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusDown( Point stylus )
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusUp( Point stylus )
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::StylusMove( Point stylus )
{
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonDown(int button)
{
	HandleKeyEvents(WM_KEYDOWN, button, 0L);


	/*
	m_bQuit = true;
	::PostQuitMessage(0);
	Shutdown();
	*/

	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonUp(int button)
{
	HandleKeyEvents(WM_KEYUP, button, 0L);
	return true;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SendKey(int iKeyCode, int iEventType)
{ 
	if(!m_pBDCommandProcessor->m_bClientConnected)
		return;

	BDCommand *pCommand = new BD_PC_KeyWasPressed(iKeyCode, iEventType);
	m_pBDCommandProcessor->AddCommand(pCommand);

	BD_WhatDoYouHave *pBD_WhatDoYouHave = new BD_WhatDoYouHave();
	m_pBDCommandProcessor->AddCommand(pBD_WhatDoYouHave);
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int iPK_Button = 0;

    if(uMsg == WM_KEYDOWN)
	{
		g_pPlutoLogger->Write(LV_STATUS,"Key down %d", wParam);
	}

	switch (wParam)
    {
        case '0':       iPK_Button = BUTTON_0_CONST;     break;
        case '1':       iPK_Button = BUTTON_1_CONST;     break;
        case '2':       iPK_Button = BUTTON_2_CONST;     break;
        case '3':       iPK_Button = BUTTON_3_CONST;     break;
        case '4':       iPK_Button = BUTTON_4_CONST;     break;
        case '5':       iPK_Button = BUTTON_5_CONST;     break;
        case '6':       iPK_Button = BUTTON_6_CONST;     break;
        case '7':       iPK_Button = BUTTON_7_CONST;     break;
        case '8':       iPK_Button = BUTTON_8_CONST;     break;
        case '9':       iPK_Button = BUTTON_9_CONST;     break;

		case 27:		iPK_Button = BUTTON_Phone_C_CONST;				break;
		case 91:		iPK_Button = BUTTON_Phone_Pencil_CONST;			break;
		case 114:		iPK_Button = BUTTON_Phone_Talk_CONST;			break;
		case 115:		iPK_Button = BUTTON_Phone_End_CONST;			break;
		case 112:		iPK_Button = BUTTON_Phone_Soft_left_CONST;		break;
		case 113:		iPK_Button = BUTTON_Phone_Soft_right_CONST;		break;
		case 104:       iPK_Button = BUTTON_Asterisk_CONST;				break;
		case 105:       iPK_Button = BUTTON_Pound_CONST;				break;

		//TODO!!! create logic to generate BUTTON_Terminate_Text_CONST or BUTTON_Enter_CONST
		//iPK_Button = BUTTON_Terminate_Text_CONST;		break;
	}

	if(uMsg == WM_KEYUP)
		SendKey(iPK_Button ? iPK_Button : - wParam, 2);
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::TryToUpdate()
{
	if(m_bQuit)
		return;

	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	GetDisplay()->Update();
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::ShowImage(int iImageType, int iSize, char *pData)
{
	if(m_bQuit)
		return;
	
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Surface *pSurface = LoadImage(GetDisplay(), (uint8_t*)pData, (uint8_t*)(pData + iSize));

	if(pSurface)
	{
	    GetDisplay()->Blit( 0, 0, pSurface);
		GetDisplay()->Update();

		delete pSurface;
	}
}
//---------------------------------------------------------------------------------------------------------

