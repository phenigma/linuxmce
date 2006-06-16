#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include "ExtensionManager.h"
#include "Texture/GLFontRenderer.h"
#include "Mesh/MeshFrame.h"

#include "PlutoUtils/MultiThreadIncludes.h"

class OpenGL3DEngine
{
	pluto_pthread_mutex_t SceneMutex;
	MeshFrame Desktop;
	int StartTick;
	bool AnimationRemain;

	MeshFrame* CurrentLayer;
	MeshFrame* OldLayer;
	MeshFrame* SelectedFrame;
	MeshFrame* HighLightFrame;
	
public:
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);
	
	void AddMeshFrameToDesktop(MeshFrame* Frame);
	
	virtual void NewScreen();

	virtual void Paint();

	virtual void Select(PlutoRectangle* SelectedArea);
	virtual void UnSelect();
	virtual void Highlight(PlutoRectangle* HightlightArea, OpenGLGraphic* HighSurface);
	virtual void UnHighlight();

	virtual int GetTick();

};

#endif
