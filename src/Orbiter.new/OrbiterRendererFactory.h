#ifndef __ORBITER_RENDERER_FACTORY__
#define __ORBITER_RENDERER_FACTORY__

#include "Orbiter.h"

class OrbiterRenderer;

class OrbiterRendererFactory
{
public:
	static OrbiterRenderer *CreateRenderer();
};

#endif //__ORBITER_RENDERER_FACTORY__

