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
  SceneMutex("scene mutex")
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}

	SceneMutex.Init(NULL);

	//TODO: do somthing with me here!!!
	CurrentLayer = new MeshFrame();
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

	TextureManager::Instance()-> ConvertImagesToTextures();
	GL.SetClearColor(.0f, .50f, 0.0f);
	GL.ClearScreen(true, true);

	MeshTransform Transform;
	//TODO: need desktop for effects Desktop
	CurrentLayer->Paint(Transform);

	g_pPlutoLogger->Write(LV_CRITICAL, "Painting using layer %p, num layers %d", CurrentLayer, Desktop.Children.size());

	GL.Flip();
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
	CurrentLayer = new MeshFrame();
	Desktop.AddChild(CurrentLayer);

	g_pPlutoLogger->Write(LV_CRITICAL, "Current layer is now %p", CurrentLayer);
}

void OpenGL3DEngine::AddMeshFrameToDesktop(MeshFrame* Frame)
{
	PLUTO_SAFETY_LOCK(sm, SceneMutex);
	CurrentLayer->AddChild(Frame);

	g_pPlutoLogger->Write(LV_WARNING, "Adding child %p to layer %p", Frame, CurrentLayer);
}
