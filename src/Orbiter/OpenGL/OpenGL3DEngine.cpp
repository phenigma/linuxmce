#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"

#include "DCE/Logger.h"
using namespace DCE;

OpenGL3DEngine::OpenGL3DEngine()
	: OldLayer(NULL),
	CurrentLayer(NULL),
	Quit(false),
	HighLightFrame(NULL),
	SelectedFrame(NULL),
	SceneMutex("scene mutex")
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	SceneMutex.Init(NULL);

	//TODO: do somthing with me here!!!
	//CurrentLayer = new MeshFrame();
}

OpenGL3DEngine::~OpenGL3DEngine()
{
	pthread_mutex_destroy(&SceneMutex.mutex);
}

void OpenGL3DEngine::Finalize(void)
{
	SDL_Quit();
}

void OpenGL3DEngine::Paint()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;
	TextureManager::Instance()->ReleaseTextures();
	TextureManager::Instance()->ConvertImagesToTextures();

	if(CurrentLayer->Children.size() == 0)
	{
	    g_pPlutoLogger->Write(LV_CRITICAL, "NOTHING to paint (current layer: %p)", CurrentLayer);		
		return;
	}

	GL.SetClearColor(.0f, .0f, 0.0f);
	GL.ClearScreen(true, false);
	GL.EnableZBuffer(false);

	//TODO: need desktop for effects Desktop
	
	MeshTransform Transform;

	Transform.ApplyTranslate(-GL.Width/2, 0, -(GL.Width)/2);
	int Tick;
	if(this->AnimationRemain)
	{
		Tick = GetTick() - this->StartTick;
		if(Tick>5*90)
			AnimationRemain = false;
	}
	else
		Tick = 5*90;
	
		Transform.ApplyRotateY(Tick / 5);
		//Transform.ApplyTranslate(0, 0, GL.Width/2);

	if(OldLayer)
		OldLayer->SetTransform(Transform);
	
	Transform.ApplyRotateY(-90);
	CurrentLayer->SetTransform(Transform);

	Transform.SetIdentity();
	Transform.Translate(0, -GL.Height/2, (GL.Width+GL.Height)/2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	Desktop.Paint(Transform);
	g_pPlutoLogger->Write(LV_CRITICAL, "Painting using layer %p, num layers %d", CurrentLayer, 
		Desktop.Children.size());

	GL.Flip();
}

void OpenGL3DEngine::AddMeshFrameToDesktop(MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;

	CurrentLayer->AddChild(Frame);

	g_pPlutoLogger->Write(LV_WARNING, "Adding child %p to layer %p", Frame, CurrentLayer);
}

/*virtual*/ void OpenGL3DEngine::Select(PlutoRectangle* SelectedArea)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;

	if(NULL == SelectedArea)
		return;
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	//MB.SetTexture(HighSurface);
	MB.SetAlpha(0.5f);
	MB.SetColor(1.0f, 0.0f, 0.0f);
	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()), 
		float(SelectedArea->Top()), 
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Right()),
		float(SelectedArea->Top()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(
		float(SelectedArea->Left()),
		float(SelectedArea->Bottom()),
		0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(
		float(SelectedArea->Right()), 
		float(SelectedArea->Bottom()),
		0);

	UnSelect();
	SelectedFrame = new MeshFrame();
	SelectedFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(SelectedFrame);
}

/*virtual*/ void OpenGL3DEngine::Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;

	if(NULL == HightlightArea)
		return;
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetTexture(HighSurface);
	MB.SetAlpha(0.4f);
	MB.SetColor(1.0f, 1.0f, 0.0f);
	MB.SetTexture2D(
		float(HightlightArea->Left()/GL.Width),
		float(HightlightArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(HightlightArea->Left()), 
		float(HightlightArea->Top()), 
		0);

	MB.SetTexture2D(
		float(HightlightArea->Right()/GL.Width),
		float(HightlightArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(
		float(HightlightArea->Right()),
		float(HightlightArea->Top()),
		0);

	MB.SetTexture2D(
		float(HightlightArea->Left()/GL.Width),
		float(HightlightArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(
		float(HightlightArea->Left()),
		float(HightlightArea->Bottom()),
		0);

	MB.SetTexture2D(
		float(HightlightArea->Right()/GL.Width),
		float(HightlightArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(
		float(HightlightArea->Right()), 
		float(HightlightArea->Bottom()),
		0);

	UnHighlight();
	HighLightFrame = new MeshFrame();
	HighLightFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(HighLightFrame);
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{
	CurrentLayer->RemoveChild(HighLightFrame);
	delete HighLightFrame;
	HighLightFrame = NULL;
}

/*virtual*/ void OpenGL3DEngine::UnSelect()
{
	CurrentLayer->RemoveChild(SelectedFrame);
	delete SelectedFrame;
	SelectedFrame = NULL;
}

/*virtual*/ int OpenGL3DEngine::GetTick()
{
	return SDL_GetTicks();
}

void OpenGL3DEngine::NewScreen()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	
	if(OldLayer)
	{
		Desktop.RemoveChild(OldLayer);
		delete OldLayer;
	}

	OldLayer = CurrentLayer;
	AnimationRemain = true;
	StartTick = GetTick();

	CurrentLayer = new MeshFrame();
	Desktop.AddChild(CurrentLayer);
	Desktop.RemoveChild(OldLayer);
	Desktop.AddChild(OldLayer);

	g_pPlutoLogger->Write(LV_CRITICAL, "Current layer is now %p", CurrentLayer);
}


