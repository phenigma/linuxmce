#ifndef __OBJECT_RENDERER_H__
#define __OBJECT_RENDERER_H__

#include "DesignObj_Orbiter.h"

class ObjectRenderer
{
	friend class DesignObj_Orbiter;

protected:

	DesignObj_Orbiter *m_pOwner;

public:

	ObjectRenderer(DesignObj_Orbiter *pOwner);
	virtual ~ObjectRenderer(void);

	virtual void RenderGraphic(PlutoRectangle rectTotal, bool bDisableAspectRatio, PlutoPoint point = PlutoPoint(0, 0));
	virtual void RenderObject(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));

protected:

	virtual bool PreRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
	virtual bool PostRenderActions(DesignObj_Orbiter *pObj_Screen, PlutoPoint point = PlutoPoint(0, 0));
};

#endif //__OBJECT_RENDERER_H__
