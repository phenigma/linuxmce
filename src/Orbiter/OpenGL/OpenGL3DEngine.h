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

	MeshFrame* CurrentLayer;
	MeshFrame* OldLayer;
	
public:
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);
	
	void AddMeshFrameToDesktop(MeshFrame* Frame);
	
	virtual void NewScreen();

	virtual void Paint();
};

#endif
