#ifndef __ORBITER_RENDERER_FACTORY__
#define __ORBITER_RENDERER_FACTORY__

namespace DCE
{
	class Orbiter;
}

using namespace DCE;
class OrbiterRenderer;

class OrbiterRendererFactory
{
public:
	static OrbiterRenderer *CreateRenderer(Orbiter *pOrbiter);
};

#endif //__ORBITER_RENDERER_FACTORY__

