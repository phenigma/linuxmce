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

	m_pInstance = this;
	m_pBDCommandProcessor = new BDCommandProcessor_Smartphone_Bluetooth();

	m_uSignalStrength = 0;
	m_bSignalStrengthScreen = 0;
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
/*virtual*/ bool OrbiterApp::PocketFrogButtonDown(int nButton)
{
	HandleKeyEvents(WM_KEYDOWN, nButton, 0L);
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonUp(int nButton)
{
	HandleKeyEvents(WM_KEYUP, nButton, 0L);
	return true;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SendKey(int nKeyCode, int nEventType)
{ 
	if(!m_pBDCommandProcessor->m_bClientConnected)
		return;

	BDCommand *pCommand = new BD_PC_KeyWasPressed(nKeyCode, nEventType);
	m_pBDCommandProcessor->AddCommand(pCommand);

	BD_WhatDoYouHave *pBD_WhatDoYouHave = new BD_WhatDoYouHave();
	m_pBDCommandProcessor->AddCommand(pBD_WhatDoYouHave);
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int nTimeDown = 0;
	static bool bIsLongKey = false;

    if(uMsg == WM_KEYDOWN)
	{
		nTimeDown = clock();
		bIsLongKey = false;
		//g_pPlutoLogger->Write(LV_STATUS,"Key down %d, time %d", wParam, nTimeDown);
	}
	else
	{
		int nTimeUp = clock();
		bIsLongKey = (nTimeUp - nTimeDown) > 300;
		//g_pPlutoLogger->Write(LV_STATUS,"Key up %d, diff %d", wParam, nTimeUp - nTimeDown);
		int bau = 4;
	}

	int nPK_Button = nPK_Button = VirtualKey2PlutoKey(wParam, bIsLongKey);
	if(IsRepeatedKey(wParam))
	{
		SendKey(nPK_Button ? nPK_Button : - wParam, uMsg == WM_KEYUP);
	}

	//TODO!!! create logic to generate BUTTON_Terminate_Text_CONST or BUTTON_Enter_CONST
	//iPK_Button = BUTTON_Terminate_Text_CONST;		break;

	if(uMsg == WM_KEYUP)
	{
		if(BUTTON_Rept_Phone_End_CONST == nPK_Button)
		{
			OnQuit();
			return;
		}

		SendKey(nPK_Button ? nPK_Button : - wParam, 2);
	}
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::OnQuit()
{
	m_bQuit = true;
	::PostQuitMessage(0);
	Shutdown();
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
void OrbiterApp::ShowImage(int nImageType, int nSize, char *pData)
{
	PROF_START();

	if(m_bQuit)
		return;
	
	PLUTO_SAFETY_LOCK(cm, m_ScreenMutex);
	Surface *pSurface = LoadImage(GetDisplay(), (uint8_t*)pData, (uint8_t*)(pData + nSize));

	if(pSurface)
	{
	    GetDisplay()->Blit( 0, 0, pSurface);
		GetDisplay()->Update();

		delete pSurface;
	}

	PROF_STOP("Show Image");
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SetSignalStrength(int uSignalStrength)
{
	m_uSignalStrength = uSignalStrength;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::SetSignalStrengthScreen(bool bSignalStrengthScreen)
{
	m_bSignalStrengthScreen = bSignalStrengthScreen;
}
//---------------------------------------------------------------------------------------------------------
void OrbiterApp::InterceptRepeatedKeys(int nKeysListSize, char *pRepeatedKeysList)
{
	m_vectRepeatedKeys.clear();

	if(nKeysListSize)
	{
		unsigned int msgpos = 0;
		string token;
		string sList = pRepeatedKeysList;

		while((token = StringUtils::Tokenize(sList, ",", msgpos)) != "") 	
		{
			m_vectRepeatedKeys.push_back(PlutoKey2VirtualKey(atoi(token.c_str())));
		}
	}
}
//---------------------------------------------------------------------------------------------------------
int OrbiterApp::PlutoKey2VirtualKey(int nPlutoKey)
{
	int nVirtualKey = 0;

	switch (nVirtualKey)
    {
        case BUTTON_0_CONST:  nVirtualKey = '0';    break;
        case BUTTON_1_CONST:  nVirtualKey = '1';    break;
        case BUTTON_2_CONST:  nVirtualKey = '2';    break;
        case BUTTON_3_CONST:  nVirtualKey = '3';    break;
        case BUTTON_4_CONST:  nVirtualKey = '4';    break;
        case BUTTON_5_CONST:  nVirtualKey = '5';    break;
        case BUTTON_6_CONST:  nVirtualKey = '6';    break;
        case BUTTON_7_CONST:  nVirtualKey = '7';    break;
        case BUTTON_8_CONST:  nVirtualKey = '8';    break;
        case BUTTON_9_CONST:  nVirtualKey = '9';    break;

		case BUTTON_Phone_C_CONST: 			nVirtualKey = VK_ESCAPE;	break;
		case BUTTON_Phone_Pencil_CONST:		nVirtualKey = VK_LWIN;		break;
		case BUTTON_Phone_Talk_CONST: 		nVirtualKey = VK_F3;		break;
		case BUTTON_Phone_End_CONST: 		nVirtualKey = VK_F4;		break;
		case BUTTON_Phone_Soft_left_CONST:	nVirtualKey = VK_F2;		break;
		case BUTTON_Phone_Soft_right_CONST: nVirtualKey = VK_F1; 		break;
		case BUTTON_Asterisk_CONST:			nVirtualKey = VK_NUMPAD8;	break;
		case BUTTON_Pound_CONST:			nVirtualKey = VK_NUMPAD9;	break;

		case BUTTON_Up_Arrow_CONST:			nVirtualKey = VK_UP; 		break;
		case BUTTON_Down_Arrow_CONST:		nVirtualKey = VK_DOWN;		break;
		case BUTTON_Left_Arrow_CONST:		nVirtualKey = VK_LEFT;		break;
		case BUTTON_Right_Arrow_CONST:		nVirtualKey = VK_RIGHT;		break;
	}

	return nPlutoKey;
}
//---------------------------------------------------------------------------------------------------------
int OrbiterApp::VirtualKey2PlutoKey(int nVirtualKey, bool bLongKey)
{
	int nPK_Button = -nVirtualKey;

	if(!bLongKey)
	{
		switch (nVirtualKey)
		{
			case '0':       nPK_Button = BUTTON_0_CONST;     break;
			case '1':       nPK_Button = BUTTON_1_CONST;     break;
			case '2':       nPK_Button = BUTTON_2_CONST;     break;
			case '3':       nPK_Button = BUTTON_3_CONST;     break;
			case '4':       nPK_Button = BUTTON_4_CONST;     break;
			case '5':       nPK_Button = BUTTON_5_CONST;     break;
			case '6':       nPK_Button = BUTTON_6_CONST;     break;
			case '7':       nPK_Button = BUTTON_7_CONST;     break;
			case '8':       nPK_Button = BUTTON_8_CONST;     break;
			case '9':       nPK_Button = BUTTON_9_CONST;     break;

			case VK_ESCAPE:		nPK_Button = BUTTON_Phone_C_CONST;			break;
			case VK_LWIN:		nPK_Button = BUTTON_Phone_Pencil_CONST;		break;
			case VK_F3:			nPK_Button = BUTTON_Phone_Talk_CONST;		break;
			case VK_F4:			nPK_Button = BUTTON_Phone_End_CONST;		break;
			case VK_F2:			nPK_Button = BUTTON_Phone_Soft_left_CONST;	break;
			case VK_F1:			nPK_Button = BUTTON_Phone_Soft_right_CONST;	break;
			case VK_NUMPAD8:    nPK_Button = BUTTON_Asterisk_CONST;			break;
			case VK_NUMPAD9:    nPK_Button = BUTTON_Pound_CONST;			break;

			case VK_UP:			nPK_Button = BUTTON_Up_Arrow_CONST;			break;
			case VK_DOWN:		nPK_Button = BUTTON_Down_Arrow_CONST;		break;
			case VK_LEFT:		nPK_Button = BUTTON_Left_Arrow_CONST;		break;
			case VK_RIGHT:		nPK_Button = BUTTON_Right_Arrow_CONST;		break;

			//case 117: //Smartphone's VOL_UP
			//case 118: //Smartphone's VOL_DOWN
		}
	}
	else
	{
		switch (nVirtualKey)
		{
			case '0':       nPK_Button = BUTTON_Rept_0_CONST;     break;
			case '1':       nPK_Button = BUTTON_Rept_1_CONST;     break;
			case '2':       nPK_Button = BUTTON_Rept_2_CONST;     break;
			case '3':       nPK_Button = BUTTON_Rept_3_CONST;     break;
			case '4':       nPK_Button = BUTTON_Rept_4_CONST;     break;
			case '5':       nPK_Button = BUTTON_Rept_5_CONST;     break;
			case '6':       nPK_Button = BUTTON_Rept_6_CONST;     break;
			case '7':       nPK_Button = BUTTON_Rept_7_CONST;     break;
			case '8':       nPK_Button = BUTTON_Rept_8_CONST;     break;
			case '9':       nPK_Button = BUTTON_Rept_9_CONST;     break;

			case VK_ESCAPE:		nPK_Button = BUTTON_Rept_Phone_C_CONST;			break;
			case VK_LWIN:		nPK_Button = BUTTON_Rept_Phone_Pencil_CONST;	break;
			case VK_F3:			nPK_Button = BUTTON_Rept_Phone_Talk_CONST;		break;
			case VK_F4:
			case 198:			nPK_Button = BUTTON_Rept_Phone_End_CONST;		break;
			case VK_F2:			nPK_Button = BUTTON_Rept_Phone_Soft_left_CONST;	break;
			case VK_F1:			nPK_Button = BUTTON_Rept_Phone_Soft_right_CONST;break;
			case VK_NUMPAD8:    nPK_Button = BUTTON_Rept_Asterisk_CONST;		break;
			case VK_NUMPAD9:    nPK_Button = BUTTON_Rept_Pound_CONST;			break;
		}
	}

	return nPK_Button;
}
//---------------------------------------------------------------------------------------------------------
bool OrbiterApp::IsRepeatedKey(int nVirtualKey)
{
	vector<int>::iterator it;
	for(it = m_vectRepeatedKeys.begin(); it != m_vectRepeatedKeys.end(); it++)
	{
		if(*it == nVirtualKey)
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------