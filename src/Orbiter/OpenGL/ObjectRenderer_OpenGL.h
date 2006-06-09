#ifndef ObjectRenderer_OpenGL_H_
#define ObjectRenderer_OpenGL_H_

#include "../ObjectRenderer.h"

class ObjectRenderer_OpenGL : public ObjectRenderer
{
public:

	ObjectRenderer_OpenGL(DesignObj_Orbiter *pOwner);
	virtual ~ObjectRenderer_OpenGL(void);

	virtual void RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));
//	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
};

#endif
