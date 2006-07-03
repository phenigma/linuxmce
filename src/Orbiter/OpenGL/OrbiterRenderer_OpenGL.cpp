#include "OrbiterRenderer_OpenGL.h"
//-----------------------------------------------------------------------------------------------------
#ifdef WIN32
	#include "MainDialog.h"
	#include "PopupMessage.h"
	#include "ProgressDialog.h"
#endif	
//-----------------------------------------------------------------------------------------------------
#include "Orbiter.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "../ScreenHistory.h"
#include "../../pluto_main/Define_Button.h"
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
#include "Texture/GLFontRenderer.h"
//-----------------------------------------------------------------------------------------------------
#include "../../pluto_main/Define_Effect.h"
//-----------------------------------------------------------------------------------------------------
void *OrbiterRenderer_OpenGLThread(void *p)
{
	OrbiterRenderer_OpenGL * pOrbiterRenderer = (OrbiterRenderer_OpenGL *)p;
	if(NULL == pOrbiterRenderer)
		throw string("OpenGL thread started, but Orbiter is null!");

	int Width = pOrbiterRenderer->OrbiterLogic()->m_iImageWidth;
	int Height = pOrbiterRenderer->OrbiterLogic()->m_iImageHeight;

	pOrbiterRenderer->Engine = new OpenGL3DEngine();
	if(!pOrbiterRenderer->Engine->GL.InitVideoMode(Width, Height, 32, false)
	)
	{
		pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
		std::cout << "*** Thread -- GL.InitVideoMode FAILED" << std::endl;
		return NULL;
	}
	else 
	{
		std::cout << "*** Thread -- GL.InitVideoMode SUCCEEDED" << std::endl;
	}

	pOrbiterRenderer->InitializeAfterSetVideoMode();

	pOrbiterRenderer->Engine->Setup();

	pthread_cond_broadcast(&(pOrbiterRenderer->Condition));
	pOrbiterRenderer->Engine->Paint();

	while(pOrbiterRenderer->Engine && !pOrbiterRenderer->Engine->Quit)
	{
#ifdef WIN32	
		SDL_Event SDL_Event_Pending;
		while(SDL_PollEvent(&SDL_Event_Pending) && !pOrbiterRenderer->Engine->Quit)
			SDL_PushEvent(&SDL_Event_Pending);
#endif			

		if(pOrbiterRenderer->NeedToUpdateScreen())
		{
			if(!pOrbiterRenderer->Engine->NeedUpdateScreen())
				pOrbiterRenderer->ScreenUpdated();
			pOrbiterRenderer->Engine->Paint();
		}
		Sleep(15);
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
	std::cout << "*** OrbiterRenderer_OpenGL::OrbiterRenderer_OpenGL()" << std::endl;
	GLThread = 0;
	pthread_cond_init(&Condition, NULL);
	Mutex.Init(NULL, &Condition);
	m_spPendingGLEffects.reset(new PendingGLEffects());

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

	SetupWindow();
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
	MeshBuilder* Builder = new MeshBuilder();
	Builder->Begin(MBMODE_TRIANGLES);

	Builder->SetColor(color.R() / 255.0f, color.G() / 255.0f, color.B() / 255.0f);

	Builder->SetTexture2D(0.0f, 0.0f);
	Builder->AddVertexFloat(
		float(x), 
		float(y), 
		0
		);
	Builder->SetTexture2D(0.0f, 1.0f);
	Builder->AddVertexFloat(
		float(x), 
		float(y+height), 
		0
		);
	Builder->SetTexture2D(1.0f, 0);
	Builder->AddVertexFloat(
		float(x+width), 
		float(y), 
		0
		);

	Builder->SetTexture2D(0.0f, 1.0f);
	Builder->AddVertexFloat(
		float(x), 
		float(y+height), 
		0
		);
	Builder->SetTexture2D(1.0f, 0);
	Builder->AddVertexFloat(
		float(x+width), 
		float(y), 
		0
		);
	Builder->SetTexture2D(1.0f, 1.0f);
	Builder->AddVertexFloat(
		float(x+width), 
		float(y+height), 
		0
		);

	MeshContainer* Container = Builder->End();

	MeshFrame* Frame = new MeshFrame(Container);

	//TODO: find a way to replace an existing object if the graphic is changed instead of adding it
	Engine->AddMeshFrameToDesktop("", Frame);


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
	//g_pPlutoLogger->Write(LV_CRITICAL, "Rendering text %s at %d, %d", sTextToDisplay.c_str(), 
	//	Text->m_rPosition.X, Text->m_rPosition.Y);

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

	
	GLFontRenderer * aGLTextRenderer = new GLFontRenderer(OrbiterLogic()->m_iImageHeight, 
			pTextStyle->m_sFont,
			pTextStyle->m_iPixelHeight,
			style, 
			pTextStyle->m_ForeColor.R(), 
			pTextStyle->m_ForeColor.G(), 
			pTextStyle->m_ForeColor.B()
		);

 	MeshFrame *Frame  = aGLTextRenderer->TextOut(sTextToDisplay, Text, pTextStyle, point);
	
	Engine->AddMeshFrameToDesktop("", Frame);
	
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
	//g_pPlutoLogger->Write(LV_CRITICAL, "(5) Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);

	OpenGLGraphic* Graphic = dynamic_cast<OpenGLGraphic*> (pPlutoGraphic);

	MeshBuilder* Builder = new MeshBuilder();
	Builder->Begin(MBMODE_TRIANGLES);

	Builder->SetColor(1.0f, 1.0f, 1.0f);
	Builder->SetTexture(Graphic);

	Builder->SetTexture2D(0.0f, 0.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Left()), 
		float(point.Y + rectTotal.Top()), 
		0
		);
	Builder->SetTexture2D(0.0f, 1.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Left()), 
		float(point.Y + rectTotal.Bottom()), 
		0
		);
	Builder->SetTexture2D(1.0f, 1.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Right()), 
		float(point.Y + rectTotal.Bottom()), 
		0
		);

	Builder->SetTexture2D(0.0f, 0.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Left()), 
		float(point.Y + rectTotal.Top()), 
		0
		);
	Builder->SetTexture2D(1.0f, 0.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Right()), 
		float(point.Y + rectTotal.Top()), 
		0
		);
	Builder->SetTexture2D(1.0f, 1.0f);
	Builder->AddVertexFloat(
		float(point.X + rectTotal.Right()), 
		float(point.Y + rectTotal.Bottom()), 
		0
		);


	//g_pPlutoLogger->Write(LV_CRITICAL, "(6) Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);

	MeshContainer* Container = Builder->End();

	MeshFrame* Frame = new MeshFrame();
	Frame->SetMeshContainer(Container);

	MeshTransform Transform;
	
	//g_pPlutoLogger->Write(LV_CRITICAL, "(7) Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);
	
	TextureManager::Instance()->PrepareConvert(Graphic);

	//g_pPlutoLogger->Write(LV_CRITICAL, "(8) Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);
	
	g_pPlutoLogger->Write(LV_STATUS, "AddMeshFrameToDesktop (%d,%d,%d,%d)",
		point.X, point.Y, rectTotal.Width, rectTotal.Height);

	//g_pPlutoLogger->Write(LV_CRITICAL, "(9) Rendering graphic size (%d, %d)", rectTotal.Width, rectTotal.Height);
		
	//TODO: find a way to replace an existing object if the graphic is changed instead of adding it
	Engine->AddMeshFrameToDesktop("", Frame);
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
	if(sbNoSelection == OrbiterLogic()->m_nSelectionBehaviour || !OrbiterLogic()->m_pObj_Highlighted || !OrbiterLogic()->m_pObj_Highlighted->m_bOnScreen )
		return;

	UnHighlightObject();

	if( !OrbiterLogic()->m_pObj_Highlighted )
		return;

	DesignObj_Orbiter *pObj = OrbiterLogic()->m_pObj_Highlighted;

	Engine->Highlight(&pObj->m_rPosition, NULL);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::SelectObject( DesignObj_Orbiter *pObj, PlutoPoint point )
{
		int EffectCode = GLEffect2D::EffectFactory::GetEffectCode(rand()%9);

		Engine->Select(&pObj->m_rPosition);
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
/*virtual*/ void OrbiterRenderer_OpenGL::RenderObjectAsync(DesignObj_Orbiter *pObj)
{
	OrbiterRenderer::RenderObjectAsync(pObj);
	//pObj->RenderObject(OrbiterLogic()->m_pScreenHistory_Current->GetObj());
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
int OrbiterRenderer_OpenGL::PromptUser(string sPrompt,int iTimeoutSeconds,map<int,string> *p_mapPrompts)
{
#ifdef WIN32
	return PromptUserEx(sPrompt, p_mapPrompts);
#endif
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterRenderer_OpenGL::DisplayProgress(string sMessage, int nProgress)
{
#ifdef WIN32
	return DialogProgressEx(sMessage, nProgress);
#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OrbiterRenderer_OpenGL::RenderScreen(bool bRenderGraphicsOnly)
{
	if(OrbiterLogic()->m_pObj_SelectedLastScreen)
		m_spPendingGLEffects->m_nOnSelectWithChangeEffectID = 
		OrbiterLogic()->m_pObj_SelectedLastScreen->m_FK_Effect_Selected_WithChange;



	int a = m_spPendingGLEffects->m_nOffScreenTransitionEffectID;
	int b = m_spPendingGLEffects->m_nOnScreenTransitionEffectID;
	int c = m_spPendingGLEffects->m_nOnSelectWithChangeEffectID;

	g_pPlutoLogger->Write(LV_WARNING,"OrbiterRenderer_OpenGL::RenderScreen");

	PlutoRectangle rectLastSelected(0, 0, 0, 0);

	if(OrbiterLogic()->m_pObj_SelectedLastScreen)
	{
		rectLastSelected = PlutoRectangle(OrbiterLogic()->m_pObj_SelectedLastScreen->m_rPosition);
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "No object selected? :o");
		rectLastSelected.X = 0;
		rectLastSelected.Y = 0;
		rectLastSelected.Width = 80;
		rectLastSelected.Height = 80;
	}

	Engine->NewScreen();
	OrbiterRenderer::RenderScreen(bRenderGraphicsOnly);

	
	NeedToUpdateScreen_ = true;

	GLEffect2D::Effect* Item;
	srand((unsigned int)time(NULL));
	//int EffectCode = GL2D_EFFECT_FADES_FROM_TOP;
	//int EffectCode = GL2D_EFFECT_BEZIER_TRANSIT;
	int EffectCode = GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	//int EffectCode = GLEffect2D::EffectFactory::GetEffectCode(9);
	//g_pPlutoLogger->Write(LV_WARNING, "%d", EffectCode);
	a = GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	b = GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	c = GLEffect2D::EffectFactory::GetEffectCode(rand()%9);
	
	Item = Engine->Compose->CreateEffect(2, a, 0, 1200);
	if(Item)
	{
		Item->Configure(&rectLastSelected);
		Item->Reversed = true;
	}
	Item = Engine->Compose->CreateEffect(3, b, 0, 1200);
	if(Item)
		Item->Configure(&rectLastSelected);
}

void OrbiterRenderer_OpenGL::InitializeAfterSetVideoMode()
{
//	SetupWindow();
}
