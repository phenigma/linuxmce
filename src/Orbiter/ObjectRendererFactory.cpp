#include "ObjectRendererFactory.h"
#include "ObjectRenderer.h"
#include "DataGridRenderer.h"
#include "../pluto_main/Define_DesignObjType.h"

#if defined(BLUETOOTH_DONGLE)
	#include "../Bluetooth_Dongle/DataGridRenderer_Bluetooth.h"
#elif defined(PROXY_ORBITER)
	#include "../Proxy_Orbiter/DataGridRenderer_Proxy.h"
#endif

#if defined(ORBITER_OPENGL)
	#include "OpenGL/ObjectRenderer_OpenGL.h"
#endif

ObjectRendererFactory::ObjectRendererFactory()
{
}

ObjectRendererFactory::~ObjectRendererFactory()
{
}

/*static*/ ObjectRenderer *ObjectRendererFactory::CreateObjectRenderer(DesignObj_Orbiter *pOwner, int nObjectType)
{
	switch(nObjectType)
	{
		case DESIGNOBJTYPE_Datagrid_CONST:

#if defined(BLUETOOTH_DONGLE)
			return new DataGridRenderer_Bluetooth(pOwner);
#elif defined(PROXY_ORBITER)
			return new DataGridRenderer_Proxy(pOwner);
#else
			return new DataGridRenderer(pOwner);
#endif

	}

#if defined(ORBITER_OPENGL)
	return new ObjectRenderer_OpenGL(pOwner);
#else
	return new ObjectRenderer(pOwner);
#endif
}
