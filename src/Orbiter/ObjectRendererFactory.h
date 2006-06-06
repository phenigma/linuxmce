#ifndef __OBJECT_RENDERER_FACTORY_H__
#define __OBJECT_RENDERER_FACTORY_H__

class ObjectRenderer;
class DesignObj_Orbiter;

class ObjectRendererFactory
{
public:
	ObjectRendererFactory();
	~ObjectRendererFactory();

	static ObjectRenderer *CreateObjectRenderer(DesignObj_Orbiter *pOwner, int nObjectType);
};

#endif //__OBJECT_RENDERER_FACTORY_H__
