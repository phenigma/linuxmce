#include "OrbiterRenderer_OpenGL.h"
//-----------------------------------------------------------------------------------------------------
#include "SelfUpdate.h"
#include "MainDialog.h"
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "../ScreenHistory.h"
//-----------------------------------------------------------------------------------------------------
#include "math3dutils.h"
#include "Widgets/basicwindow.h"
#include "Widgets/DrawingWidgetsEngine.h"
#include "Effects/gl2deffecttransit.h"
#include "Effects/gl2deffectbeziertranzit.h"
#include "Effects/gl2deffectslidefromleft.h"
#include "Effects/gl2deffectfadesfromtop.h"
#include "Effects/gl2deffectfadesfromunderneath.h"
#include "Orbiter3DCommons.h" 
#include "Simulator.h" 
#include "OpenGLGraphic.h"
#include "OpenGL3DEngine.h"
#include "Mesh/MeshFrame.h"
#include "Mesh/MeshBuilder.h"
//-----------------------------------------------------------------------------------------------------
#include <SDL_ttf.h>
#include "Texture/GLFontTextureList.h"
//-----------------------------------------------------------------------------------------------------
#include "../../pluto_main/Define_Effect.h"
//-----------------------------------------------------------------------------------------------------
void *OrbiterRenderer_OpenGLThread(void *p)
{
	OrbiterRenderer_OpenGL * pOrbiterRenderer = (OrbiterRenderer_OpenGL *)p;
	if(NULL == pOrbiterRenderer)
		throw string("OpenGL thread started, but Orbiter is null!");

	pOrbiterRenderer->Engine = new OpenGL3DEngine();
	if(!pOrbiterRenderer->Engine->GL.InitVideoMode(
		pOrbiterRenderer->OrbiterLogic()->m_iImageWidth, 
		pOrbiterRenderer->OrbiterLogic()->m_iImageHeight, 32, false
		)
	)
	{
		pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
		return NULL;
	}

	pOrbiterRenderer->Engine->GL.Setup();

	pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
	pOrbiterRenderer->Engine->Paint();

	while(pOrbiterRenderer->Engine && !pOrbiterRenderer->Engine->Quit)
	{
		SDL_Event SDL_Event_Pending;
		while(SDL_PollEvent(&SDL_Event_Pending) && !pOrbiterRenderer->Engine->Quit)
			SDL_PushEvent(&SDL_Event_Pending);

		if(pOrbiterRenderer->NeedToUpdateScreen())
		{
			pOrbiterRenderer->Engine->Paint();
			pOrbiterRenderer->ScreenUpdated();
		}

		//pOrbiterRenderer->Engine->GetDesktop().CleanUp();
		Sleep(10);
	}

	pOrbiterRenderer->Engine->Finalize();	
	delete pOrbiterRenderer->Engine;
	pOrbiterRenderer->Engine = NULL;

	return NULL; 
}	
//-----------------------------------------------------------------------------------------------------
OrbiterRenderer_OpenGL::OrbiterRenderer_OpenGL(Orbiter *pOrbiter) : 
	OrbiterRenderer(pOrbiter), Mutex("open gl"), Engine(NULL), NeedToUpdateScreen_(false)
{
	GLThread = 0;
	pthread_cond_init(&Condition, NULL);
	Mutex.Init(NULL, &Condition);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ OrbiterRenderer_OpenGL::~OrbiterRenderer_OpenGL()
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::Configure()
{
	pthread_cond_init(&Condition, NULL);
	Mutex.Init(NULL, &Condition);

	pthread_create(&GLThread, NULL, OrbiterRenderer_OpenGLThread, (void*)this);	

	PLUTO_SAFETY_LOCK_ERRORSONLY(cm, Mutex);// Keep this locked to protect the map
	cm.CondWait(); 
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::GetWindowPosition(PlutoPoint& point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UpdateScreen()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SolidRectangle(int x, int y, int width, int height, PlutoColor color)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::HollowRectangle(int X, int Y, int Width, int Height, PlutoColor color)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ReplaceColorInRectangle(int x, int y, int width, int height, PlutoColor ColorToReplace, PlutoColor ReplacementColor)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderText(string &sTextToDisplay, DesignObjText *Text,
	TextStyle *pTextStyle, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{
	g_pPlutoLogger->Write(LV_CRITICAL, "Rendering text %s", sTextToDisplay.c_str());

	if(sTextToDisplay.length() == 0)
		return;


	int style = TTF_STYLE_NORMAL;
	if (pTextStyle->m_bUnderline)
	{
		style = TTF_STYLE_UNDERLINE;
	}
	else
	{
		if (pTextStyle->m_bBold)
			style |= TTF_STYLE_BOLD;
		if (pTextStyle->m_bItalic)
			style |= TTF_STYLE_ITALIC;
	}

	GLFontTextureList* aGLFontTextureList = new GLFontTextureList(OrbiterLogic()->m_iImageHeight);
	aGLFontTextureList->MapFont(pTextStyle->m_sFont, pTextStyle->m_iPixelHeight, style, pTextStyle->m_ForeColor.R(), 
		pTextStyle->m_ForeColor.G(), pTextStyle->m_ForeColor.B());
	MeshContainer *Container = aGLFontTextureList->TextOut(point.X + Text->m_rPosition.X, 
		point.Y + Text->m_rPosition.Y, const_cast<char *>(sTextToDisplay.c_str()));
	MeshFrame *Frame = new MeshFrame();
	Frame->SetMeshContainer(Container);
	Engine->AddMeshFrameToDesktop(Frame);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SaveBackgroundForDeselect(DesignObj_Orbiter *pObj, PlutoPoint point)
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *OrbiterRenderer_OpenGL::GetBackground( PlutoRectangle &rect ) 
{
	//todo
	return NULL;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ PlutoGraphic *OrbiterRenderer_OpenGL::CreateGraphic()
{
	return new OpenGLGraphic(this);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderGraphic(class PlutoGraphic *pPlutoGraphic, PlutoRectangle rectTotal, 
	bool bDisableAspectRatio, PlutoPoint point/* = PlutoPoint(0, 0)*/)
{
	g_pPlutoLogger->Write(LV_CRITICAL, "Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);

	OpenGLGraphic* Graphic = dynamic_cast<OpenGLGraphic*> (pPlutoGraphic);

	MeshBuilder* Builder = new MeshBuilder();
	Builder->Begin(MBMODE_TRIANGLE_STRIP);

	Builder->SetColor(1.0f, 1.0f, 1.0f);
	Builder->SetTexture(Graphic);

	Builder->SetTexture2D(0.0f, 0.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Left()), 
		float(point.Y + rectTotal.Top()), 
		float(OrbiterLogic()->m_iImageHeight / 2)
		);
	Builder->SetTexture2D(Graphic->MaxU, 0);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Right()), 
		float(point.Y + rectTotal.Top()), 
		float(OrbiterLogic()->m_iImageHeight / 2)
		);
	Builder->SetTexture2D(0.0f, Graphic->MaxV);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Left()), 
		float(point.Y + rectTotal.Bottom()), 
		float(OrbiterLogic()->m_iImageHeight / 2)
		);
	Builder->SetTexture2D(Graphic->MaxU, Graphic->MaxV);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Right()), 
		float(point.Y + rectTotal.Bottom()), 
		float(OrbiterLogic()->m_iImageHeight / 2)
		);

	MeshContainer* Container = new MeshContainer();
	Container = Builder->End();

	MeshFrame* Frame = new MeshFrame();
	Frame->SetMeshContainer(Container);
	
	TextureManager::Instance()->PrepareImage(Graphic);

	g_pPlutoLogger->Write(LV_STATUS, "AddMeshFrameToDesktop %d - (%d,%d,%d,%d)",
		point.X, point.Y, rectTotal.Width, rectTotal.Height);

	//TODO: find a way to replace an existing object if the graphic is changed instead of adding it
	Engine->AddMeshFrameToDesktop(Frame);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::EndPaint()
{
	NeedToUpdateScreen_ = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::UpdateRect(PlutoRectangle rect, PlutoPoint point/*= PlutoPoint(0,0)*/)
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::AdjustWindowSize(int iWidth, int iHeight)
{

}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_OpenGL::WakeupFromCondWait()
{
	pthread_cond_broadcast(&Condition);
}
//-----------------------------------------------------------------------------------------------------
void OrbiterRenderer_OpenGL::OnIdle()
{
	//WakeupFromCondWait();
	Sleep(5);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::DoHighlightObject()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point )
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::EventLoop()
{
	int SDL_Event_Pending = 0;

	SDL_Event Event;

	// For now I'll assume that shift + arrows scrolls a grid
	while (!OrbiterLogic()->m_bQuit && !OrbiterLogic()->m_bReload)
	{
		SDL_Event_Pending = SDL_PollEvent(&Event);

		if (SDL_Event_Pending)
		{
			Orbiter::Event orbiterEvent;
			orbiterEvent.type = Orbiter::Event::NOT_PROCESSED;

			if (Event.type == SDL_QUIT)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Received sdl event SDL_QUIT");
				break;
			} 
			else if(Event.type == SDL_MOUSEMOTION)
			{
				orbiterEvent.type = Orbiter::Event::MOUSE_MOVE;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			} 
			else if (Event.type == SDL_MOUSEBUTTONDOWN)
			{
				orbiterEvent.type = Orbiter::Event::REGION_DOWN;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				OrbiterLogic()->ProcessEvent(orbiterEvent);

#if defined(WIN32) && !defined(PROXY_ORBITER) && !defined(BLUETOOTH_DONGLE)
				RecordMouseAction(Event.button.x, Event.button.y);
#endif
			}
			else if (Event.type == SDL_MOUSEBUTTONUP)
			{
				orbiterEvent.type = Orbiter::Event::REGION_UP;
				orbiterEvent.data.region.m_iX = Event.button.x;
				orbiterEvent.data.region.m_iY = Event.button.y;
				OrbiterLogic()->ProcessEvent(orbiterEvent);
			}
		}
		else
		{
			OnIdle();
		}
	}  // while
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::OnQuit()
{
	g_pPlutoLogger->Write(LV_WARNING,"Got an on quit.  Pushing an event into SDL");
	SDL_Event *pEvent = new SDL_Event;
	pEvent->type = SDL_QUIT;
	SDL_PushEvent(pEvent);
	delete pEvent;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::Destroy()
{
	//cleanup here
	if(NULL != Engine)
		Engine->Quit = true;

	if(GLThread != 0)
		pthread_join(GLThread, NULL);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderScreen(bool bRenderGraphicsOnly)
{
	Engine->NewScreen();
	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);

	NeedToUpdateScreen_ = true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderObjectAsync(DesignObj_Orbiter *pObj)
{
	pObj->RenderObject(OrbiterLogic()->m_pScreenHistory_Current->GetObj());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ShowProgress(int nPercent) 
{
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool OrbiterRenderer_OpenGL::NeedToUpdateScreen()
{
	return NeedToUpdateScreen_;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::ScreenUpdated()
{
	NeedToUpdateScreen_ = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RedrawObjects()
{
	OrbiterRenderer::RedrawObjects();
	NeedToUpdateScreen_ = true;
}
//-----------------------------------------------------------------------------------------------------