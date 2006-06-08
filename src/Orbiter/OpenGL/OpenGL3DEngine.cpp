#include "OpenGL3DEngine.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL_ttf.h>

#include "Mesh/MeshPainter.h"

OpenGL3DEngine::OpenGL3DEngine()
{
	if(TTF_Init()==-1) {
		printf("Error on TTF_Init: %s\n", TTF_GetError());
		return;
	}
	Quit = false;
}

OpenGL3DEngine::~OpenGL3DEngine()
{
}

void OpenGL3DEngine::Finalize(void)
{
	SDL_Quit();
}

void OpenGL3DEngine::Paint()
{
	GL.SetClearColor(.0f, .50f, 0.0f);
	GL.ClearScreen(true, true);

	MeshTransform Transform;
	Desktop.Paint(Transform);
	GL.Flip();
}

/*virtual*/ MeshFrame& OpenGL3DEngine::GetDesktop()
{
	return Desktop;
}
