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
  HighLight(NULL),
  Selected(NULL),
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
	TextureManager::Instance()-> ConvertImagesToTextures();

	if(CurrentLayer->Children.size() == 0)
	{
	    g_pPlutoLogger->Write(LV_CRITICAL, "NOTHING to paint (current layer: %p)", CurrentLayer);		
		return;
	}

	GL.SetClearColor(.0f, .00f, 0.0f);
	GL.ClearScreen(true, false);
	GL.EnableZBuffer(false);

	//TODO: need desktop for effects Desktop
	
	/*MeshTransform Transform;
	Transform.SetIdentity();
	Transform.SetIdentity();
	Transform.Translate(0, 0, GL.Height/2);
	Transform.RotateY(Angle);
	CurrentLayer->ApplyTransform(Transform);
	*/
/*

	Transform.ApplyTranslate(-GL.Width/2, -GL.Height/2, -GL.Width/2);
	static float Angle = -2.3f;
	Angle += 2.3f;
	Transform.ApplyRotateY(45+Angle);
	Transform.ApplyTranslate(GL.Width/2, GL.Height/2, GL.Width/2);
	CurrentLayer->SetTransform(Transform);
	*/
	MeshTransform Transform;
	Desktop.Paint(Transform);

	g_pPlutoLogger->Write(LV_CRITICAL, "Painting using layer %p, num layers %d", CurrentLayer, 
		Desktop.Children.size());

	GL.Flip();
}

void OpenGL3DEngine::NewScreen()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);

	if(CurrentLayer)
		Desktop.RemoveChild(CurrentLayer);

	CurrentLayer = new MeshFrame();
	MeshTransform Transform;
	Transform.Translate(-GL.Width/2, -GL.Height/2, GL.Height/2);
	Desktop.SetTransform(Transform);
	Desktop.AddChild(CurrentLayer);

	g_pPlutoLogger->Write(LV_CRITICAL, "Current layer is now %p", CurrentLayer);
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
	MB.AddVertexFloat(SelectedArea->Left(), SelectedArea->Top(), 0);

	MB.SetTexture2D(
		float(SelectedArea->Left()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(SelectedArea->Left(), SelectedArea->Bottom(), 0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(SelectedArea->Right(), SelectedArea->Top(), 0);

	MB.SetTexture2D(
		float(SelectedArea->Right()/GL.Width),
		float(SelectedArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(SelectedArea->Right(), SelectedArea->Bottom(), 0);

	CurrentLayer->RemoveChild(Selected);
	delete Selected;
	Selected = new MeshFrame();
	Selected->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(Selected);
}

/*virtual*/ void OpenGL3DEngine::Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;
	CurrentLayer->RemoveChild(HighLight);
	delete HighLight;

	if(NULL == HightlightArea)
		return;
	HighLight = new MeshFrame();
	MeshBuilder MB;
	MB.Begin(MBMODE_TRIANGLE_STRIP);
	MB.SetTexture(HighSurface);
	MB.SetAlpha(0.4f);
	MB.SetColor(1.0f, 1.0f, 0.0f);
	MB.SetTexture2D(
		float(HightlightArea->Left()/GL.Width),
		float(HightlightArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(HightlightArea->Left(), HightlightArea->Top(), 0);

	MB.SetTexture2D(
		float(HightlightArea->Left()/GL.Width),
		float(HightlightArea->Bottom()/GL.Height)
		);
	MB.AddVertexFloat(HightlightArea->Left(), HightlightArea->Bottom(), 0);
	
	MB.SetTexture2D(
		float(HightlightArea->Right()/GL.Width),
		float(HightlightArea->Top()/GL.Height)
		);
	MB.AddVertexFloat(HightlightArea->Right(), HightlightArea->Top(), 0);

	MB.SetTexture2D(
		float(HightlightArea->Right()/GL.Width),
		float(HightlightArea->Bottom()/GL.Height)
		);

	MB.AddVertexFloat(HightlightArea->Right(), HightlightArea->Bottom(), 0);
	HighLight->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(HighLight);
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{

}
