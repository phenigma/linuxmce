#ifndef OpenGL3DEngine_H_
#define OpenGL3DEngine_H_

#include "ExtensionManager.h"
#include "Texture/GLFontRenderer.h"
#include "Mesh/MeshFrame.h"

class OpenGL3DEngine
{
	//TODO: use me!
	//pthread_cond_t SleepCond;
	//pthread_mutex_t SleepMutex;
	MeshFrame Desktop;	
	
public:
	bool Quit;
	ExtensionManager GL;
	OpenGL3DEngine(void);
	virtual ~OpenGL3DEngine();
	virtual void Finalize(void);
	
	virtual MeshFrame& GetDesktop();
	
	virtual void Paint();
};

#endif
