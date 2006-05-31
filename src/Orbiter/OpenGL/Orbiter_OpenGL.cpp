#include "Orbiter_OpenGL.h"
//-----------------------------------------------------------------------------------------------------
#include "SelfUpdate.h"
#include "MainDialog.h"
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
//-----------------------------------------------------------------------------------------------------
#include "math3dutils.h"
#include "GL2DWidgets/basicwindow.h"
#include "GL2DWidgets/DrawingWidgetsEngine.h"
#include "orbitergl3dengine.h"
#include "GL2DEffects/gl2deffecttransit.h"
#include "GL2DEffects/gl2deffectbeziertranzit.h"
#include "GL2DEffects/gl2deffectslidefromleft.h"
#include "GL2DEffects/gl2deffectfadesfromtop.h"
#include "GL2DEffects/gl2deffectfadesfromunderneath.h"
#include "Orbiter3DCommons.h" 
#include "Simulator.h" 
#include "OpenGLGraphic.h"

#include "../../pluto_main/Define_Effect.h"
//-----------------------------------------------------------------------------------------------------
Orbiter_OpenGL *Orbiter_OpenGL::m_pInstance = NULL; //the one and only
//-----------------------------------------------------------------------------------------------------
void *Orbiter_OpenGLThread(void *p)
{
	Orbiter_OpenGL * pOrbiter = (Orbiter_OpenGL *)p;
	if(NULL == pOrbiter)
		throw string("OpenGL thread started, but Orbiter is null!");

	// Create the OpenGL window and classes asociated with it
	// OpenGL drawing operations, locked in the mutex
	PLUTO_SAFETY_LOCK(cm, *(pOrbiter->m_GLThreadMutex));

	pOrbiter->m_Desktop = OrbiterGL3D::GetInstance();
	pOrbiter->m_Desktop->BuildOrbiterGL(pOrbiter);

	//cm.CondWait(); // This will unlock the mutex and lock it on awakening

	pOrbiter->PaintDesktopGL = false;

	while(!pOrbiter->m_bQuit)
	{
		//hack to allow sdl to work from other thread ?
		SDL_Event Event;
		while(SDL_PollEvent(&Event))
			SDL_PushEvent(&Event);

		cm.CondWait();	

		if (!pOrbiter->m_Desktop->EffectBuilder->HasEffects()) 
		{
			if (!pOrbiter->PaintDesktopGL) 
			{
				pOrbiter->PaintDesktopGL = true;
				pOrbiter->m_Desktop->Paint();
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
			pOrbiter->PaintDesktopGL = false;
			pOrbiter->m_Desktop->Paint();
		}

		Sleep(10);
	}

	delete pOrbiter->m_Desktop;
	pOrbiter->m_Desktop = NULL;

	return NULL;
}	
//-----------------------------------------------------------------------------------------------------
Orbiter_OpenGL::Orbiter_OpenGL(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode, 
							   int nImageWidth, int nImageHeight, bool bFullScreen/*= false*/, 
							   pluto_pthread_mutex_t *pExternalScreenMutex/*=NULL*/)
	: Orbiter(DeviceID, PK_DeviceTemplate, ServerAddress, sLocalDirectory, bLocalMode, nImageWidth,
	nImageHeight, pExternalScreenMutex)
{
	/// if OpenGL is used there is created one OpenGL drawing mutex
	m_GLThreadMutex = new pluto_pthread_mutex_t("open gl worker thread");
	OpenGLThreadId = 0;

	pthread_cond_init(&m_GLThreadCond, NULL);
	m_GLThreadMutex->Init(NULL, &m_GLThreadCond);
	m_Desktop = NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Orbiter_OpenGL::~Orbiter_OpenGL()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_OpenGL::GetConfig()
{
	if(!Orbiter::GetConfig())
		return false;

	pthread_create(&OpenGLThreadId, NULL, Orbiter_OpenGLThread, (void*)this);

	/*
//////////////////////////////////////////////////////////////////////////
		Uint32 uSDLInitFlags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;

		if(m_bFullScreen)
			uSDLInitFlags |= SDL_FULLSCREEN;

		if (SDL_Init(uSDLInitFlags) == -1)
		{
			printf("Failed to initialize SDL %s\n", SDL_GetError());
			exit(1);
		}

		SDL_WM_SetCaption("OrbiterOpenGL", "OrbiterOpenGL");

		atexit(SDL_Quit);
		g_pPlutoLogger->Write(LV_STATUS, "Initialized SDL");

		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		SDL_SetVideoMode(m_iImageWidth, m_iImageHeight, 0, SDL_OPENGL | SDL_RESIZABLE);
		g_pPlutoLogger->Write(LV_STATUS, "Created back screen surface!");

//////////////////////////////////////////////////////////////////////////
		*/

	return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::GetWindowPosition(PlutoPoint& point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::PingFailed()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::UpdateScreen()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::SolidRectangle(int x, int y, int width, int height, PlutoColor color)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::RenderText(string &sTextToDisplay,class DesignObjText *Text,class TextStyle *pTextStyle, 
	PlutoPoint point/* = PlutoPoint(0, 0)*/)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::RenderText(HDC hdc,string &sTextToDisplay,PlutoRectangle &rPosition,int iPK_HorizAlignment,int iPK_VertAlignment,
	string &sFont,PlutoColor &ForeColor,int iPixelHeight,bool bBold,bool bItalic,bool bUnderline, PlutoPoint point, RECT &rectLocation)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *Orbiter_OpenGL::GetBackground( PlutoRectangle &rect ) 
{
	//todo
	return NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *Orbiter_OpenGL::CreateGraphic()
{
	return new OpenGLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::RenderScreen( bool bRenderGraphicsOnly )
{
	Orbiter::RenderScreen(bRenderGraphicsOnly);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::RedrawObjects()
{
    PLUTO_SAFETY_LOCK(cm,m_ScreenMutex);
	Orbiter::RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
	bool bDisableAspectRatio, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::EndPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::UpdateRect(PlutoRectangle rect, PlutoPoint point/*= PlutoPoint(0,0)*/)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::ShowProgress(int nPercent)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::OnQuit()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ int Orbiter_OpenGL::PromptUser(string sPrompt, int iTimeoutSeconds/*= 10*/,
	map<int,string> *p_mapPrompts/*= NULL*/)
{
	//todo
	return 0;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Orbiter_OpenGL::DisplayProgress(string sMessage, int nProgress)
{	
	//todo
    return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::Initialize(GraphicType Type, int iPK_Room/*= 0*/, 
	int iPK_EntertainArea/* = 0*/)
{
    Orbiter::Initialize(Type, iPK_Room, iPK_EntertainArea);
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_OpenGL::Cleanup()
{
	g_pPlutoLogger->Write(LV_STATUS, "Orbiter_OpenGL: need to cleanup orbiter...");

	if(NULL != m_pInstance)
	{
		/*
		//todo for SDL

		if(::IsWindow(m_pInstance->m_hWnd))
			::DestroyWindow(m_pInstance->m_hWnd);
		*/

		m_pInstance->m_bQuit = true;	

		Orbiter_OpenGL *pCopy = m_pInstance;
		m_pInstance = NULL;

		delete pCopy;
		pCopy = NULL;

		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_OpenGL: orbiter deleted.");
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ void Orbiter_OpenGL::BuildOrbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress, string sLocalDirectory, bool bLocalMode,
	int nImageWidth, int nImageHeight, bool bFullScreen/*= false*/, bool bUseOpenGL /*= false*/,
	pluto_pthread_mutex_t *pExternalScreenMutex/*=NULL*/)
{
	if(NULL == m_pInstance)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Orbiter_OpenGL constructor.");
		m_pInstance = new Orbiter_OpenGL(DeviceID, PK_DeviceTemplate, ServerAddress, 
			sLocalDirectory, bLocalMode, nImageWidth, 
			nImageHeight, bFullScreen, pExternalScreenMutex);
		
		//g_pCommand_Impl = m_pInstance;  
	}
	else
	{
		throw "Orbiter_PocketFrog already created!";
	}
}
//-----------------------------------------------------------------------------------------------------
/*static*/ Orbiter_OpenGL *Orbiter_OpenGL::GetInstance()
{
	if(!m_pInstance)
		g_pPlutoLogger->Write(LV_STATUS, "The instance of orbiter is NULL");

	return m_pInstance;
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_OpenGL::WriteStatusOutput(const char* pMessage)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::OnUnexpectedDisconnect()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::AdjustWindowSize(int iWidth, int iHeight)
{

}
//-----------------------------------------------------------------------------------------------------
void Orbiter_OpenGL::CalcTextRectangle(RECT &rectLocation,PlutoRectangle &rPosition,int iRotation,int iHeight,int iVertAlignment)
{

}
//-----------------------------------------------------------------------------------------------------
bool Orbiter_OpenGL::SelfUpdate()
{
	OrbiterSelfUpdate orbiterSelfUpdate(this);
	return orbiterSelfUpdate.Run();
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_OpenGL::WakeupFromCondWait()
{
	pthread_cond_broadcast(&m_GLThreadCond);
}
//-----------------------------------------------------------------------------------------------------
void Orbiter_OpenGL::OnIdle()
{
	WakeupFromCondWait();
	Sleep(5);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::DoHighlightObject()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Orbiter_OpenGL::SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point )
{

}
//-----------------------------------------------------------------------------------------------------

