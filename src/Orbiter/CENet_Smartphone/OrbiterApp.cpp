#include "OrbiterApp.h"

#include "DCE/Logger.h"
#include "BD/BDCommandProcessor_Smartphone_Bluetooth.h"
using namespace DCE;

#include <string>
using namespace std;

#define APP_WIDTH  176
#define APP_HEIGHT 220
//---------------------------------------------------------------------------------------------------------
CComModule _Module; 
//---------------------------------------------------------------------------------------------------------
OrbiterApp::OrbiterApp() : m_ScreenMutex("rendering")
{
	m_bQuit = false;

	m_iImageWidth = APP_WIDTH;
	m_iImageHeight = APP_HEIGHT;

    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );
	m_ScreenMutex.Init( &m_MutexAttr );

	//m_pBDCommandProcessor = new BDCommandProcessor_Smartphone_Bluetooth();
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterApp::~OrbiterApp()
{
	pthread_mutexattr_destroy(&m_MutexAttr);
	pthread_mutex_destroy(&m_ScreenMutex.mutex);

	//delete m_pBDCommandProcessor;
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
	m_bQuit = true;
	::PostQuitMessage(0);
	Shutdown();

	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterApp::PocketFrogButtonUp(int button)
{
	return true;
}
//---------------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterApp::HandleKeyEvents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
