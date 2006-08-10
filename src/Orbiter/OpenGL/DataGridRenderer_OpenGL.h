#ifndef DataGridRenderer_OpenGL_H_
#define DataGridRenderer_OpenGL_H_


#include "DataGridRenderer.h"
#include "PlutoUtils/MultiThreadIncludes.h"

class OpenGL3DEngine;
class DesignObj_Orbiter;
class MeshFrame;
class DataGridRenderer_OpenGL :
	public DataGridRenderer
{
	OpenGL3DEngine* Engine;
	MeshFrame* RenderFrame;
	//pluto_pthread_mutex_t LockDrawer;
	int StartAnimation;
	int iPK_Direction;

public:
	DataGridRenderer_OpenGL(DesignObj_Orbiter *pOwner);
	virtual ~DataGridRenderer_OpenGL(void);


	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual void Scroll_Grid(string sRelative_Level, int iPK_Direction);
};

#endif
