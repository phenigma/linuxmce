#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"
#include "Layers/GL2DEffectLayersCompose.h"
#include "Texture/TextureManager.h"
#include "Texture/GLFontManager.h"

#include "DCE/Logger.h"
using namespace DCE;

OpenGL3DEngine::OpenGL3DEngine()
	: OldLayer(NULL),
	CurrentLayer(NULL),
	Quit(false),
	HighLightFrame(NULL),
	SelectedFrame(NULL),
	AnimationRemain (false),
	SceneMutex("scene mutex"),
	Compose(NULL)
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	SceneMutex.Init(NULL);
}

OpenGL3DEngine::~OpenGL3DEngine()
{
	MeshPainter::Instance()->CleanUp();
	GLEffect2D::LayersCompose::Instance()->CleanUp();
	TextureManager::Instance()->CleanUp();
	GLFontManager::GetInstance()->CleanUp();

	CurrentLayerObjects_.clear();
	TTF_Quit();

	pthread_mutex_destroy(&SceneMutex.mutex);
}

void OpenGL3DEngine::Finalize(void)
{
	SDL_Quit();
}

bool OpenGL3DEngine::Paint()
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return false;

	if(Compose->HasEffects())
		AnimationRemain = true;
	
	TextureManager::Instance()->ReleaseTextures();
	TextureManager::Instance()->ConvertImagesToTextures();

	if(CurrentLayer->Children.size() == 0)
	{
	    g_pPlutoLogger->Write(LV_CRITICAL, "NOTHING to paint (current layer: %p)", CurrentLayer);		
		return false;
	}

	GL.SetClearColor(.0f, .0f, 0.5f);
	//GL.ClearScreen(true, false);
		
	GL.EnableZBuffer(false);

	Compose->UpdateLayers(CurrentLayer, OldLayer);
	Compose->Paint();
	if(HighLightFrame)
	{
		Point3D Color;
		Color.X = 1.0f;
		Color.Y = 1.0f;
		Color.Z = (GetTick() / 2 % 512) / 255.0f;
		Color.Z = abs(Color.Z - 1.0f)/2.0f+ 0.5f;
		Color.X = Color.Z;
		Color.Y = Color.Z;
		CurrentLayer->RemoveChild(HighLightFrame);
		CurrentLayer->AddChild(HighLightFrame);
		HighLightFrame->GetMeshContainer()->SetColor(Color);
	}

	//glEnable(GL_CULL_FACE);

	GL.Flip();


	bool Status = NULL != Compose && Compose->HasEffects() && AnimationRemain;
	if (Compose->HasEffects() == false)
		AnimationRemain = false;

	return Status;
}

/*virtual*/ int OpenGL3DEngine::GetTick()
{
	return SDL_GetTicks();
}

void OpenGL3DEngine::NewScreen()
{
	UnHighlight();
	UnSelect();

	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	
	if(NULL != OldLayer)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "NewScreen, removing stuff...");
		Desktop.RemoveChild(OldLayer);
		delete OldLayer;
	}

	OldLayer = CurrentLayer;
	
	StartTick = GetTick();

	CurrentLayerObjects_.clear();
	CurrentLayer = new MeshFrame();
	Desktop.AddChild(CurrentLayer);
	
	Desktop.RemoveChild(OldLayer);
	Desktop.AddChild(OldLayer);

	g_pPlutoLogger->Write(LV_CRITICAL, "Current layer is now %p, size %d", 
		CurrentLayer, Desktop.Children.size());
}

void OpenGL3DEngine::Setup()
{
	GL.Setup();
	Compose = GLEffect2D::LayersCompose::Instance();
	Compose->Setup(GL.Width, GL.Height);
}

void OpenGL3DEngine::AddMeshFrameToDesktop(string ObjectID, MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	if(NULL == CurrentLayer)
		return;

	if(ObjectID != "")
	{
		std::map<string, MeshFrame *>::iterator it = CurrentLayerObjects_.find(ObjectID);
		if(it != CurrentLayerObjects_.end())
		{
			MeshFrame* OldFrame = it->second;
			g_pPlutoLogger->Write(LV_CRITICAL, "Replacing child %p, object %s of layer %p", 
				OldFrame, ObjectID.c_str(), CurrentLayer);

			CurrentLayer->RemoveChild(OldFrame);
		}
	}

	CurrentLayer->AddChild(Frame);

	if(ObjectID != "")
	{
		CurrentLayerObjects_[ObjectID] = Frame;
	}

	g_pPlutoLogger->Write(LV_WARNING, "Adding child %p, object %s to layer %p", Frame, ObjectID.c_str(), CurrentLayer);
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
	MeshTransform Transform;
	Compose->PaintScreen3D();
	Compose->NewScreen->RenderFrameToGraphic();
	MB.SetTexture(Compose->NewScreen->GetRenderGraphic());
	//MB.SetAlpha(0.4f);
	//MB.SetColor(1.0f, 1.0f, 0.0f);

	MB.SetTexture2D(
		float(HightlightArea->Left())/GL.Width,
		1- float(HightlightArea->Top())/GL.Height
		);
	MB.AddVertexFloat(0, 0, 0);

	MB.SetTexture2D(
		float(HightlightArea->Right())/GL.Width,
		1- float(HightlightArea->Top())/GL.Height
		);
	MB.AddVertexFloat(1, 0, 0);

	MB.SetTexture2D(
		float(HightlightArea->Left())/GL.Width,
		1- float(HightlightArea->Bottom())/GL.Height
		);
	MB.AddVertexFloat(0, 1, 0);
	MB.SetTexture2D(
		float(HightlightArea->Right())/GL.Width,
		1- float(HightlightArea->Bottom())/GL.Height
		);

	MB.AddVertexFloat(1, 1, 0);

	UnHighlight();
	HighLightFrame = new MeshFrame();
	Transform.ApplyTranslate(-0.1f, -0.1f, 0.f);
	Transform.ApplyScale(1.2f, 1.2f, 1.0f);
	Transform.ApplyScale(
		float(HightlightArea->Width),
		float(HightlightArea->Height),
		1.0f);
	Transform.ApplyTranslate(
		float(HightlightArea->Left()),
		float(HightlightArea->Top()),
		0.f);


	HighLightFrame->SetTransform(Transform);
	HighLightFrame->SetMeshContainer(MB.End());
	CurrentLayer->AddChild(HighLightFrame);
}

/*virtual*/ void OpenGL3DEngine::UnHighlight()
{
	if(HighLightFrame == NULL)
		return;
	CurrentLayer->RemoveChild(HighLightFrame);

	delete HighLightFrame;
	HighLightFrame = NULL;
	
}

/*virtual*/ void OpenGL3DEngine::UnSelect()
{
	if(SelectedFrame == NULL)
		return;
	CurrentLayer->RemoveChild(SelectedFrame);

	delete SelectedFrame;
	SelectedFrame = NULL;
}

bool OpenGL3DEngine::NeedUpdateScreen()
{
	return Compose->HasEffects() || HighLightFrame != NULL;
}

